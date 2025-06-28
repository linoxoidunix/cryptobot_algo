#pragma once
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/system/error_code.hpp>
#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "aoe/response_queue_listener/i_response_queue_listener.h"
#include "aoe/session/web_socket/i_web_socket.h"
#include "aos/logger/mylog.h"
#include "boost/asio.hpp"
#include "boost/asio/awaitable.hpp"
#include "boost/asio/this_coro.hpp"

namespace beast     = boost::beast;          // from <boost/beast.hpp>
namespace http      = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket;      // from <boost/beast/websocket.hpp>
namespace net       = boost::asio;           // from <boost/asio.hpp>
namespace ssl       = boost::asio::ssl;      // from <boost/asio/ssl.hpp>
using tcp           = boost::asio::ip::tcp;  // from <boost/asio/ip/tcp.hpp>

namespace aoe {
namespace impl {
class WebSocketSessionRW : public WebSocketSessionWritableInterface,
                           public WebSocketSessionReadableInterface {
    bool session_ready_ = false;
    /**
     * @brief req variable must manage only via SetRequest() method
     *
     */
    tcp::resolver resolver_;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> stream_;
    beast::flat_buffer buffer_;  // (Must persist between reads)
    boost::asio::io_context& ioc_;
    std::string_view host_;
    std::string_view port_;
    std::string_view default_endpoint_;
    boost::asio::steady_timer timer_;  // Timer to track session expiration
    // signal to cancel all coroutines
    boost::asio::cancellation_signal cancel_signal_;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::strand<boost::asio::io_context::executor_type> write_strand_;
    boost::asio::thread_pool thread_pool_;
    moodycamel::ConcurrentQueue<nlohmann::json> message_queue_;
    moodycamel::ConcurrentQueue<std::vector<char>>& response_queue_;
    ResponseQueueListenerInterface& listener_;

  public:
    explicit WebSocketSessionRW(
        boost::asio::io_context& ioc, ssl::context& ctx,
        const std::string_view host, const std::string_view port,
        const std::string_view default_endpoint,
        moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
        ResponseQueueListenerInterface& listener)
        : resolver_(net::make_strand(ioc)),
          stream_(net::make_strand(ioc), ctx),
          ioc_(ioc),
          strand_(net::make_strand(ioc)),
          // write_strand_(net::make_strand(ioc)),
          write_strand_(net::make_strand(ioc)),
          host_(host),
          port_(port),
          default_endpoint_(default_endpoint),
          timer_(ioc),  // Initialize the timer
          response_queue_(response_queue),
          listener_(listener) {
        auto fut = net::co_spawn(
            ioc,
            [&]() -> net::awaitable<void> {
                try {
                    logi("run session");
                    co_await Run(host.data(), port.data(),
                                 default_endpoint.data());
                } catch (const std::exception& e) {
                    loge("Error: {}", e.what());
                }
            },
            net::use_future);
        fut.get();  // блокирует, ждет завершения
    }
    void AsyncWrite(nlohmann::json&& message) override {
        net::dispatch(strand_, [this, message = std::move(message)]() {
            message_queue_.enqueue(message);
            StartWrite();
        });
    };
    ~WebSocketSessionRW() override = default;
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return response_queue_;
    }

  private:
    net::awaitable<void> Run(const char* host, const char* port,
                             const char* default_end_point) {
        beast::error_code ec;

        auto results = co_await resolver_.async_resolve(
            host, port, net::redirect_error(net::use_awaitable, ec));
        if (ec) {
            loge("Failed to resolve host '{}:{}': {}", host, port,
                 ec.message());
            co_return;
        }
        if (!SSL_set_tlsext_host_name(stream_.next_layer().native_handle(),
                                      host)) {
            auto ec = beast::error_code(static_cast<int>(::ERR_get_error()),
                                        net::error::get_ssl_category());
            loge("{}", ec.message());
            co_return;
        }
        // start_timer();
        auto ep = co_await beast::get_lowest_layer(stream_).async_connect(
            results, net::redirect_error(net::use_awaitable, ec));
        if (ec) {
            loge("Failed to connect to endpoint: {}", ec.message());
            co_return;
        }
        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        beast::get_lowest_layer(stream_).expires_never();

        co_await stream_.next_layer().async_handshake(
            ssl::stream_base::client,
            net::redirect_error(net::use_awaitable, ec));
        if (ec) {
            loge("SSL handshake failed: {}", ec.message());
            co_return;
        }
        // Set suggested timeout settings for the websocket
        stream_.set_option(websocket::stream_base::timeout::suggested(
            beast::role_type::client));
        // Set a decorator to change the User-Agent of the handshake
        stream_.set_option(
            websocket::stream_base::decorator([](websocket::request_type& req) {
                req.set(http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-client-async-ssl");
            }));
        co_await stream_.async_handshake(
            host, default_end_point,
            net::redirect_error(net::use_awaitable, ec));
        if (ec) {
            loge("WebSocket handshake failed: {}", ec.message());
            co_return;
        }
        timer_.cancel();
        session_ready_ = true;
        co_spawn(ioc_, WriteLoop(), net::detached);
        co_spawn(ioc_, ReadLoop(), net::detached);
        co_return;
    }
    net::awaitable<void> ReadLoop() {
        // beast::flat_buffer buffer;
        while (true) {
            logi("start async read");

            boost::system::error_code read_ec;
            std::size_t n = 0;

            // Создаём корутину для чтения, гарантируя последовательность
            // операций
            try {
                logi("try async_read");
                // Выполняем чтение данных
                auto result = co_await stream_.async_read(
                    buffer_, boost::asio::as_tuple(boost::asio::use_awaitable));
                read_ec = std::get<0>(result);
                n       = std::get<1>(result);
            } catch (const boost::system::system_error& e) {
                read_ec = e.code();
            }

            // Обрабатываем результат чтения
            if (read_ec) {
                if (read_ec == boost::asio::error::operation_aborted) {
                    CloseSessionFast();
                    co_return;
                } else {
                    loge("Read error: {}", read_ec.message());
                    CloseSessionFast();
                    co_return;
                }
            }

            logi("invoke callback received_bytes:{}", n);

            // Выполняем коллбеки только если есть данные
            if (n > 0) {
                std::string_view data_view{
                    reinterpret_cast<const char*>(buffer_.data().data()),
                    buffer_.size()};
                std::vector<char> data_copy(data_view.begin(), data_view.end());
                response_queue_.enqueue(std::move(data_copy));  // копия строки
                logi("get data queue_size:{}", response_queue_.size_approx());
                listener_.OnDataEnqueued();
                logi("add message to queue");
                buffer_.consume(buffer_.size());  // очищаем буфер
            } else {
                logd("No data was read");
            }

            // Освобождаем потребленные данные из буфера
            buffer_.consume(n);
        }

        logi("finished read");
        CloseSessionFast();
        logd("start execute cb when close session");
    }
    net::awaitable<void> WriteLoop() {
        while (true) {
            if (!message_queue_.size_approx()) {
                // Если очередь пуста, приостанавливаем выполнение
                // is_writing_ = false;
                co_return;
            }

            // Берем следующее сообщение из очереди
            nlohmann::json message;
            message_queue_.try_dequeue(message);
            auto dump = message.dump();
            // Adding a delay before writing to avoid buffer overflow on the
            // server side or the server disconnecting the stream due to sending
            // messages too frequently I chose 1 second arbitrarily as a
            // reasonable delay
            net::steady_timer timer(co_await net::this_coro::executor);
            timer.expires_after(std::chrono::seconds(1));
            co_await timer.async_wait(net::use_awaitable);

            // Выполняем запись
            try {
                co_await stream_.async_write(net::buffer(dump),
                                             net::use_awaitable);
                logi("Sent throw ws: {}", message.dump());
            } catch (const std::exception& e) {
                loge("Write error: {}", e.what());
                break;
            }
        }
    }
    void StartWrite() {
        boost::asio::co_spawn(
            ioc_,
            [this]() -> net::awaitable<void> {
                if (!session_ready_) co_return;
                co_await WriteLoop();
            },
            net::detached);
    }
    // Close the session gracefully
    void CloseSessionFast() { beast::get_lowest_layer(stream_).close(); }
};

class WebSocketSessionW : public WebSocketSessionWritableInterface {
    bool session_ready_ = false;
    /**
     * @brief req variable must manage only via SetRequest() method
     *
     */
    tcp::resolver resolver_;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> stream_;
    beast::flat_buffer buffer_;  // (Must persist between reads)
    boost::asio::io_context& ioc_;
    std::string_view host_;
    std::string_view port_;
    std::string_view default_endpoint_;
    boost::asio::steady_timer timer_;  // Timer to track session expiration
    // signal to cancel all coroutines
    boost::asio::cancellation_signal cancel_signal_;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::strand<boost::asio::io_context::executor_type> write_strand_;
    boost::asio::thread_pool thread_pool_;
    moodycamel::ConcurrentQueue<nlohmann::json> message_queue_;

  public:
    explicit WebSocketSessionW(boost::asio::io_context& ioc, ssl::context& ctx,
                               const std::string_view host,
                               const std::string_view port,
                               const std::string_view default_endpoint)
        : resolver_(net::make_strand(ioc)),
          stream_(net::make_strand(ioc), ctx),
          ioc_(ioc),
          strand_(net::make_strand(ioc)),
          // write_strand_(net::make_strand(ioc)),
          write_strand_(net::make_strand(ioc)),
          host_(host),
          port_(port),
          default_endpoint_(default_endpoint),
          timer_(ioc)  // Initialize the timer
    {
        // net::co_spawn(
        //     ioc,
        //     [&]() -> net::awaitable<void> {
        //         try {
        //             co_await Run(host.data(), port.data(),
        //                          default_endpoint.data());
        //         } catch (const std::exception& e) {
        //             loge("Error: {}", e.what());
        //         }
        //     },
        //     net::detached);
        auto fut = net::co_spawn(
            ioc,
            [&]() -> net::awaitable<void> {
                try {
                    logi("run session");
                    co_await Run(host.data(), port.data(),
                                 default_endpoint.data());
                } catch (const std::exception& e) {
                    loge("Error: {}", e.what());
                }
            },
            net::use_future);
        fut.get();  // блокирует, ждет завершения
    }
    void AsyncWrite(nlohmann::json&& message) override {
        net::dispatch(strand_, [this, message = std::move(message)]() {
            message_queue_.enqueue(message);
            StartWrite();
        });
    };
    ~WebSocketSessionW() override = default;

  private:
    net::awaitable<void> Run(const char* host, const char* port,
                             const char* default_end_point) {
        beast::error_code ec;

        auto results = co_await resolver_.async_resolve(
            host, port, net::redirect_error(net::use_awaitable, ec));
        if (ec) {
            loge("Failed to resolve host '{}:{}': {}", host, port,
                 ec.message());
            co_return;
        }
        if (!SSL_set_tlsext_host_name(stream_.next_layer().native_handle(),
                                      host)) {
            auto ec = beast::error_code(static_cast<int>(::ERR_get_error()),
                                        net::error::get_ssl_category());
            loge("{}", ec.message());
            co_return;
        }
        // start_timer();
        auto ep = co_await beast::get_lowest_layer(stream_).async_connect(
            results, net::redirect_error(net::use_awaitable, ec));
        if (ec) {
            loge("Failed to connect to endpoint: {}", ec.message());
            co_return;
        }
        // Turn off the timeout on the tcp_stream, because
        // the websocket stream has its own timeout system.
        beast::get_lowest_layer(stream_).expires_never();

        co_await stream_.next_layer().async_handshake(
            ssl::stream_base::client,
            net::redirect_error(net::use_awaitable, ec));
        if (ec) {
            loge("SSL handshake failed: {}", ec.message());
            co_return;
        }
        // Set suggested timeout settings for the websocket
        stream_.set_option(websocket::stream_base::timeout::suggested(
            beast::role_type::client));
        // Set a decorator to change the User-Agent of the handshake
        stream_.set_option(
            websocket::stream_base::decorator([](websocket::request_type& req) {
                req.set(http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                            " websocket-client-async-ssl");
            }));
        co_await stream_.async_handshake(
            host, default_end_point,
            net::redirect_error(net::use_awaitable, ec));
        if (ec) {
            loge("WebSocket handshake failed: {}", ec.message());
            co_return;
        }
        timer_.cancel();
        session_ready_ = false;
        co_spawn(ioc_, WriteLoop(), net::detached);
        co_spawn(ioc_, ReadLoop(), net::detached);
        co_return;
    }
    net::awaitable<void> ReadLoop() {
        // beast::flat_buffer buffer;
        while (true) {
            logi("start async read");

            boost::system::error_code read_ec;
            std::size_t n = 0;

            // Создаём корутину для чтения, гарантируя последовательность
            // операций
            try {
                logi("try async_read");
                // Выполняем чтение данных
                auto result = co_await stream_.async_read(
                    buffer_, boost::asio::as_tuple(boost::asio::use_awaitable));
                read_ec = std::get<0>(result);
                n       = std::get<1>(result);
            } catch (const boost::system::system_error& e) {
                read_ec = e.code();
            }

            // Обрабатываем результат чтения
            if (read_ec) {
                if (read_ec == boost::asio::error::operation_aborted) {
                    CloseSessionFast();
                    co_return;
                } else {
                    loge("Read error: {}", read_ec.message());
                    CloseSessionFast();
                    co_return;
                }
            }

            logi("invoke callback received_bytes:{}", n);

            // Выполняем коллбеки только если есть данные
            if (n > 0) {
                // skip process response
            } else {
                logd("No data was read");
            }

            // Освобождаем потребленные данные из буфера
            buffer_.consume(n);
        }

        logi("finished read");
        CloseSessionFast();
        logd("start execute cb when close session");
    }
    net::awaitable<void> WriteLoop() {
        while (true) {
            if (!message_queue_.size_approx()) {
                // Если очередь пуста, приостанавливаем выполнение
                // is_writing_ = false;
                co_return;
            }

            // Берем следующее сообщение из очереди
            nlohmann::json message;
            message_queue_.try_dequeue(message);
            auto dump = message.dump();
            // Adding a delay before writing to avoid buffer overflow on the
            // server side or the server disconnecting the stream due to sending
            // messages too frequently I chose 1 second arbitrarily as a
            // reasonable delay
            net::steady_timer timer(co_await net::this_coro::executor);
            timer.expires_after(std::chrono::seconds(1));
            co_await timer.async_wait(net::use_awaitable);

            // Выполняем запись
            try {
                co_await stream_.async_write(net::buffer(dump),
                                             net::use_awaitable);
                logi("Sent throw ws: {}", message.dump());
            } catch (const std::exception& e) {
                loge("Write error: {}", e.what());
                break;
            }
        }
    }
    void StartWrite() {
        boost::asio::co_spawn(
            strand_,
            [this]() -> net::awaitable<void> {
                if (!session_ready_) co_return;
                co_await WriteLoop();
            },
            net::detached);
    }
    // Close the session gracefully
    void CloseSessionFast() { beast::get_lowest_layer(stream_).close(); }
};
class WebSocketSessionDummy : public WebSocketSessionWritableInterface {
  public:
    void AsyncWrite(nlohmann::json&&) override {};
    ~WebSocketSessionDummy() override = default;
};
};  // namespace impl
};  // namespace aoe