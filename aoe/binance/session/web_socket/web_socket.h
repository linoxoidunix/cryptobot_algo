#pragma once

#include "aoe/binance/enums/enums.h"
#include "aoe/binance/session/web_socket/i_web_socket.h"
#include "aoe/session/web_socket/web_socket.h"

namespace aoe {
namespace binance {
namespace impl {
namespace main_net {
namespace private_channel {
class SessionRW : public WebSocketPrivateSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, "ws-api.binance.com", "443", "/ws-api/v3",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }

    ~SessionRW() override = default;
};
};  // namespace private_channel
};  // namespace main_net

namespace main_net {
namespace trade_channel {
class SessionW : public WebSocketPrivateSessionWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionW ws_;

  public:
    SessionW(boost::asio::io_context& ioc)
        : ws_(ioc, ctx_, "ws-api.binance.com", "443", "/ws-api/v3") {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    ~SessionW() override = default;
};

class SessionRW : public WebSocketPrivateSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, "ws-api.binance.com", "443", "/ws-api/v3",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }

    ~SessionRW() override = default;
};
};  // namespace trade_channel
};  // namespace main_net

namespace test_net {
namespace private_channel {
class SessionRW : public WebSocketPrivateSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, "ws-api.testnet.binance.vision", "443", "/ws-api/v3",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }

    ~SessionRW() override = default;
};
};  // namespace private_channel
};  // namespace test_net
namespace test_net {
namespace trade_channel {
class SessionW : public WebSocketPrivateSessionWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionW ws_;

  public:
    SessionW(boost::asio::io_context& ioc)
        : ws_(ioc, ctx_, "ws-api.testnet.binance.vision", "443", "/v5/trade") {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    ~SessionW() override = default;
};

class SessionRW : public WebSocketPrivateSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, "ws-api.testnet.binance.vision", "443", "/v5/trade",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }

    ~SessionRW() override = default;
};
};  // namespace trade_channel
};  // namespace test_net

namespace test_net {
namespace spot {
namespace order_book_channel {
class SessionRW : public WebSocketPublicSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;
    moodycamel::ConcurrentQueue<std::vector<char>> unused_queue_;

  public:
    SessionRW()                            = delete;
    SessionRW(const SessionRW&)            = delete;
    SessionRW(SessionRW&&)                 = delete;
    SessionRW& operator=(const SessionRW&) = delete;
    SessionRW& operator=(SessionRW&&)      = delete;
    ~SessionRW() override                  = default;
    void AsyncWrite(nlohmann::json&& j) override {};
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return unused_queue_;
    }
};
};  // namespace order_book_channel
};  // namespace spot
};  // namespace test_net

namespace test_net {
namespace futures {
namespace order_book_channel {
class SessionRW : public WebSocketPublicSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, "stream.testnet.binancefuture.com", "443", "/ws",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }

    ~SessionRW() override = default;
};
};  // namespace order_book_channel
};  // namespace futures
};  // namespace test_net

namespace main_net {
namespace spot {
namespace order_book_channel {
class SessionRW : public WebSocketPublicSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, aoe::binance::main_net::ws::spot::kWSHost1, "9443",
              "/ws", response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }

    ~SessionRW() override = default;
};
};  // namespace order_book_channel
};  // namespace spot
};  // namespace main_net

namespace main_net {
namespace futures {
namespace order_book_channel {
class SessionRW : public WebSocketPublicSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, aoe::binance::main_net::ws::futures::kWSHost1, "443",
              "/ws", response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }

    ~SessionRW() override = default;
};
};  // namespace order_book_channel
};  // namespace futures
};  // namespace main_net

};  // namespace impl
};  // namespace binance
};  // namespace aoe