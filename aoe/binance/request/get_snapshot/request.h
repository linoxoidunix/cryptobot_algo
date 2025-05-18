#pragma once
#include "aoe/binance/enum_printer/enum_printer.h"
#include "aoe/binance/enums/enums.h"
#include "aoe/binance/request/get_snapshot/i_request.h"
#include "aos/converters/trading_pair_to_big_string_view/trading_pair_to_big_string_view.h"
#include "boost/asio.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/http/message.hpp"
#include "boost/beast/version.hpp"
namespace aoe {
namespace binance {
namespace snapshot {
namespace details {
template <template <typename> typename MemoryPool>
class BaseRequest : public RequestInterface<MemoryPool> {
  protected:
    aos::impl::TradingPairToBigStringView trading_pair_printer_;
    std::string_view base_end_point_;
    std::string_view host_name_;
    BaseRequest(std::string_view endpoint, std::string_view host_name)
        : base_end_point_(endpoint), host_name_(host_name) {}

  public:
    std::pair<bool,
              boost::beast::http::request<boost::beast::http::string_body>>
    Accept(RequestMakerInterface<MemoryPool>* /*request_maker*/) override {
        boost::beast::http::request<boost::beast::http::string_body> req;
        req.version(11);
        req.method(boost::beast::http::verb::get);

        auto [status, value] =
            trading_pair_printer_.Convert(this->TradingPair());
        if (!status) return {false, {}};

        auto end_point = fmt::format("{}?symbol={}&limit={}", base_end_point_,
                                     value, this->Limit());
        req.target(end_point);
        req.set(boost::beast::http::field::host, host_name_);
        req.set(boost::beast::http::field::user_agent,
                BOOST_BEAST_VERSION_STRING);
        req.set(boost::beast::http::field::content_type,
                aoe::binance::kContentType);
        return {true, req};
    }

    ~BaseRequest() override = default;
};
}  // namespace details
namespace main_net {
namespace spot::impl {
template <template <typename> typename MemoryPool>
class Request
    : public aoe::binance::snapshot::details::BaseRequest<MemoryPool> {
  public:
    Request()
        : details::BaseRequest<MemoryPool>(
              "/api/v3/depth", aoe::binance::main_net::rest::spot::kRESTHost1) {
    }
    ~Request() override = default;
};
};  // namespace spot::impl
};  // namespace main_net

namespace main_net {
namespace futures::impl {
template <template <typename> typename MemoryPool>
class Request
    : public aoe::binance::snapshot::details::BaseRequest<MemoryPool> {
  public:
    Request()
        : details::BaseRequest<MemoryPool>(
              "/fapi/v1/depth",
              aoe::binance::main_net::rest::futures::kRESTHost1) {}
    ~Request() override = default;
};
};  // namespace futures::impl
};  // namespace main_net
};  // namespace snapshot
};  // namespace binance
};  // namespace aoe