#pragma once
#include "aoe/binance/enums/enums.h"
#include "aoe/binance/request_maker/for_rest/get_snapshot/i_request_maker.h"
#include "aos/common/common.h"
#include "aos/request/i_request.h"
#include "aos/trading_pair/trading_pair.h"
#include "aos/common/exchange_id.h"
#include "boost/asio.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/http/message.hpp"
#include "boost/beast/version.hpp"
namespace aoe {
namespace binance {
namespace snapshot {
template <template <typename> typename MemoryPool>
class RequestInterface : public aos::RequestInterface<MemoryPool> {
  public:
    virtual ~RequestInterface() = default;
    virtual common::ExchangeId ExchangeId() const { return exchange_id_; };
    virtual const aos::TradingPair& TradingPair() const {
        return trading_pair_;
    };
    virtual void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    };
    virtual int Limit() const { return limit_; }
    virtual void SetLimit(int limit) {
        if (limit > 5000) {
            limit = 5000;
        }
        limit_ = limit;
    }
    // json appearance for binance
    virtual std::pair<
        bool, boost::beast::http::request<boost::beast::http::string_body>>
    Accept(RequestMakerInterface<MemoryPool>* request_maker) = 0;

  protected:
    common::ExchangeId exchange_id_ = common::ExchangeId::kBinance;
    aos::TradingPair trading_pair_;
    // default value limit is 100 for binance
    // maximum value 5000
    // https://developers.binance.com/docs/binance-spot-api-docs/rest-api/market-data-endpoints
    int limit_ = 5000;
};
};  // namespace snapshot
};  // namespace binance
};  // namespace aoe