#pragma once

#include "aoe/bybit/session/web_socket/i_web_socket.h"
#include "aoe/session/web_socket/web_socket.h"
namespace aoe {
namespace bybit {
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
        : ws_(ioc, ctx_, "stream.bybit.com", "443", "/v5/private",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }
    void StartAsync() override { ws_.StartAsync(); }
    void StopAsync() override { ws_.StopAsync(); }
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
    explicit SessionW(boost::asio::io_context& ioc)
        : ws_(ioc, ctx_, "stream.bybit.com", "443", "/v5/trade") {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    void StartAsync() override { ws_.StartAsync(); }
    void StopAsync() override { ws_.StopAsync(); }
    ~SessionW() override = default;
};

class SessionRW : public WebSocketPrivateSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, "stream.bybit.com", "443", "/v5/trade", response_queue,
              listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }
    void StartAsync() override { ws_.StartAsync(); }
    void StopAsync() override { ws_.StopAsync(); }
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
        : ws_(ioc, ctx_, "stream-testnet.bybit.com", "443", "/v5/private",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }
    void StartAsync() override { ws_.StartAsync(); }
    void StopAsync() override { ws_.StopAsync(); }
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
    explicit SessionW(boost::asio::io_context& ioc)
        : ws_(ioc, ctx_, "stream-testnet.bybit.com", "443", "/v5/trade") {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    void StartAsync() override { ws_.StartAsync(); }
    void StopAsync() override { ws_.StopAsync(); }
    ~SessionW() override = default;
};

class SessionRW : public WebSocketPrivateSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, "stream-testnet.bybit.com", "443", "/v5/trade",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }
    void StartAsync() override { ws_.StartAsync(); }
    void StopAsync() override { ws_.StopAsync(); }
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

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, "stream-testnet.bybit.com", "443", "/v5/public/spot",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }
    void StartAsync() override { ws_.StartAsync(); }
    void StopAsync() override { ws_.StopAsync(); }
    ~SessionRW() override = default;
};
};  // namespace order_book_channel
};  // namespace spot
};  // namespace test_net

namespace test_net {
namespace linear {
namespace order_book_channel {
class SessionRW : public WebSocketPublicSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, "stream-testnet.bybit.com", "443", "/v5/public/linear",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }
    void StartAsync() override { ws_.StartAsync(); }
    void StopAsync() override { ws_.StopAsync(); }
    ~SessionRW() override = default;
};
};  // namespace order_book_channel
};  // namespace linear
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
        : ws_(ioc, ctx_, "stream.bybit.com", "443", "/v5/public/spot",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }
    void StartAsync() override { ws_.StartAsync(); }
    void StopAsync() override { ws_.StopAsync(); }
    ~SessionRW() override = default;
};
};  // namespace order_book_channel
};  // namespace spot
};  // namespace main_net

namespace main_net {
namespace linear {
namespace order_book_channel {
class SessionRW : public WebSocketPublicSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSessionRW ws_;

  public:
    SessionRW(boost::asio::io_context& ioc,
              moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
              ResponseQueueListenerInterface& listener)
        : ws_(ioc, ctx_, "stream.bybit.com", "443", "/v5/public/linear",
              response_queue, listener) {}
    void AsyncWrite(nlohmann::json&& j) override {
        ws_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return ws_.GetResponseQueue();
    }
    void StartAsync() override { ws_.StartAsync(); }
    void StopAsync() override { ws_.StopAsync(); }
    ~SessionRW() override = default;
};
};  // namespace order_book_channel
};  // namespace linear
};  // namespace main_net

};  // namespace impl
};  // namespace bybit
};  // namespace aoe