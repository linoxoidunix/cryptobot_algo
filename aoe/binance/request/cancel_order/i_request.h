#pragma once
#include "aoe/binance/enums/enums.h"
#include "aoe/binance/request_maker/for_web_socket/cancel_order/i_request_maker.h"
#include "aos/common/common.h"
#include "aos/request/i_request.h"
#include "aot/common/types.h"

namespace aoe {
namespace binance {
namespace cancel_order {
template <template <typename> typename MemoryPool>
class RequestInterface : public aos::RequestInterface<MemoryPool> {
  public:
    virtual ~RequestInterface() = default;
    virtual common::ExchangeId ExchangeId() const { return exchange_id_; };
    virtual const aos::TradingPair& TradingPair() const {
        return trading_pair_;
    };
    virtual aoe::binance::Category Category() const { return category_; };
    virtual uint64_t OrderId() const { return order_id_; };
    virtual void SetOrderId(uint64_t order_id) { order_id_ = order_id; };
    virtual void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    };
    virtual void SetCategory(aoe::binance::Category category) {
        category_ = category;
    };
    // json appearance for binance
    virtual std::pair<bool, nlohmann::json> Accept(
        aoe::binance::cancel_order::RequestMakerInterface<MemoryPool>*
            request_maker) = 0;

  protected:
    common::ExchangeId exchange_id_ = common::ExchangeId::kBybit;
    aos::TradingPair trading_pair_;
    aoe::binance::Category category_ = aoe::binance::Category::kInvalid;
    uint64_t order_id_;
    aoe::binance::OrderRouting order_routing_;
};
};  // namespace cancel_order
};  // namespace binance
};  // namespace aoe