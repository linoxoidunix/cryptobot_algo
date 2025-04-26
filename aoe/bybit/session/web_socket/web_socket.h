#pragma once

#include "aoe/bybit/session/web_socket/i_web_socket.h"
#include "aoe/session/web_socket/web_socket.h"
namespace aoe {
namespace bybit {
namespace impl {
namespace main_net {
namespace private_channel {
class Session : public WebSocketPrivateSessionInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSession ws_;

  public:
    Session(boost::asio::io_context& ioc,
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

    ~Session() override = default;
};
};  // namespace private_channel
};  // namespace main_net
namespace test_net {
namespace private_channel {
class Session : public WebSocketPrivateSessionInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::WebSocketSession ws_;

  public:
    Session(boost::asio::io_context& ioc,
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

    ~Session() override = default;
};
};  // namespace private_channel
};  // namespace test_net
};  // namespace impl
};  // namespace bybit
};  // namespace aoe