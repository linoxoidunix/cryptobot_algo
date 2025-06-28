#pragma once
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/system/error_code.hpp>
#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "aoe/response_queue_listener/i_response_queue_listener.h"
#include "aoe/session/rest/i_session.h"
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
class RestSessionRW : public RestSessionWritableInterface,
                      public RestSessionReadableInterface {
    bool session_ready_ = false;
    /**
     * @brief req variable must manage only via SetRequest() method
     *
     */
    tcp::resolver resolver_;
    beast::ssl_stream<beast::tcp_stream> stream_;
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
    moodycamel::ConcurrentQueue<
        boost::beast::http::request<boost::beast::http::string_body>>
        message_queue_;
    moodycamel::ConcurrentQueue<std::vector<char>>& response_queue_;
    ResponseQueueListenerInterface& listener_;

  public:
    explicit RestSessionRW(
        boost::asio::io_context& ioc, ssl::context& ctx,
        const std::string_view host, const std::string_view port,
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
          timer_(ioc),  // Initialize the timer
          response_queue_(response_queue),
          listener_(listener) {
        // net::co_spawn(
        //     ioc,
        //     [&]() -> net::awaitable<void> {
        //         try {
        //             co_await Run(host.data(), port.data());
        //         } catch (const std::exception& e) {
        //             loge("Error: {}", e.what());
        //         }
        //     },
        //     net::detached);
    }
    void AsyncWrite(
        boost::beast::http::request<boost::beast::http::string_body>&& message)
        override {
        logi("enqueue request");
        net::dispatch(ioc_, [this, req = std::move(message)]() mutable {
            message_queue_.try_enqueue(std::move(req));
            logd("https3 enquee message");
            // StartProcessing();
        });
    };
    ~RestSessionRW() override = default;
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return response_queue_;
    }
    void Start() {
        net::co_spawn(
            ioc_,
            [&]() -> net::awaitable<void> {
                try {
                    co_await Run(host_.data(), port_.data());
                } catch (const std::exception& e) {
                    loge("Error: {}", e.what());
                }
            },
            net::detached);
    }

  private:
    net::awaitable<void> RequestLoop() {
        logi("[https3] launch request loop");
        if (!message_queue_.size_approx()) {
            // Если очередь пуста, приостанавливаем выполнение
            logi("[https3] reject request loop due to no message in queue");
            co_return;
        }
        // while (!request_queue_.size_approx()) {
        // Берем следующий запрос из очереди
        http::request<http::string_body> req;
        message_queue_.try_dequeue(req);
        try {
            logi("[https3] send request");
            // Отправляем запрос
            co_await http::async_write(stream_, req, net::use_awaitable);
            http::response<http::string_body> res;
            // Читаем ответ
            logi("[https3] read response");
            co_await http::async_read(stream_, buffer_, res,
                                      net::use_awaitable);
            // Копируем body в вектор
            std::vector<char> data_copy(res.body().begin(), res.body().end());
            response_queue_.enqueue(std::move(data_copy));

            logi("get data queue_size:{}", response_queue_.size_approx());
            listener_.OnDataEnqueued();
            logi("add message to queue");

            buffer_.consume(buffer_.size());  // опционально, если буфер общий
        } catch (const std::exception& e) {
            loge("{}", e.what());
        }

        CloseSessionFast();
        co_return;
    }

    net::awaitable<void> Run(const char* host, const char* port) {
        logi("try init https session");
        if (!SSL_set_tlsext_host_name(stream_.native_handle(), host)) {
            loge("SSL set host error");
            CloseSessionFast();
            co_return;
        }

        boost::beast::error_code ec;
        // start_timer();

        auto results =
            co_await resolver_.async_resolve(host, port, net::use_awaitable);

        // Connect to the resolved endpoint.
        co_await beast::get_lowest_layer(stream_).async_connect(
            results, net::use_awaitable);

        // Perform the SSL handshake.
        co_await stream_.async_handshake(
            ssl::stream_base::client,
            boost::asio::redirect_error(boost::asio::use_awaitable, ec));
        if (ec) {
            CloseSessionFast();
            co_return;
        }

        timer_.cancel();
        session_ready_ = true;
        co_await RequestLoop();
        co_return;
    }
    // Close the session gracefully
    void CloseSessionFast() {
        beast::error_code ec;
        auto shutdown_ec = stream_.shutdown(ec);

        if (shutdown_ec) {
            loge("Shutdown failed: {}", shutdown_ec.message());
        }
        beast::get_lowest_layer(stream_)
            .close();  // Close the underlying transport.
    }
};

class RestSessionDummy : public RestSessionWritableInterface {
  public:
    void AsyncWrite(
        boost::beast::http::request<boost::beast::http::string_body>&&)
        override {};
    ~RestSessionDummy() override = default;
};
};  // namespace impl
};  // namespace aoe