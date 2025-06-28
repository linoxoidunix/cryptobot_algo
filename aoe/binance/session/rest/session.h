#pragma once

#include "aoe/binance/enums/enums.h"
#include "aoe/binance/session/rest/i_session.h"
#include "aoe/session/rest/session.h"
namespace aoe {
namespace binance {
namespace impl {
namespace main_net {
namespace spot {
class RestSessionRW : public RestSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::RestSessionRW session_;

  public:
    RestSessionRW(
        boost::asio::io_context& ioc,
        moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
        ResponseQueueListenerInterface& listener)
        : session_(ioc, ctx_, aoe::binance::main_net::rest::spot::kRESTHost1,
                   aoe::binance::kRESTPort, response_queue, listener) {}
    void AsyncWrite(
        boost::beast::http::request<boost::beast::http::string_body>&& j)
        override {
        session_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return session_.GetResponseQueue();
    }
    void Start() override { session_.Start(); }
    ~RestSessionRW() override = default;
};
};  // namespace spot
};  // namespace main_net
namespace main_net {
namespace futures {
class RestSessionRW : public RestSessionRWInterface {
    boost::asio::ssl::context ctx_{boost::asio::ssl::context::tlsv12_client};
    aoe::impl::RestSessionRW session_;

  public:
    RestSessionRW(
        boost::asio::io_context& ioc,
        moodycamel::ConcurrentQueue<std::vector<char>>& response_queue,
        ResponseQueueListenerInterface& listener)
        : session_(ioc, ctx_, aoe::binance::main_net::rest::futures::kRESTHost1,
                   aoe::binance::kRESTPort, response_queue, listener) {}
    void AsyncWrite(
        boost::beast::http::request<boost::beast::http::string_body>&& j)
        override {
        session_.AsyncWrite(std::move(j));
    };
    moodycamel::ConcurrentQueue<std::vector<char>>& GetResponseQueue()
        override {
        return session_.GetResponseQueue();
    }
    void Start() override { session_.Start(); }

    ~RestSessionRW() override = default;
};
};  // namespace futures
};  // namespace main_net
};  // namespace impl
};  // namespace binance
};  // namespace aoe