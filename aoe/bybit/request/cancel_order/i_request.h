#pragma once
#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/request_maker/for_web_socket/cancel_order/i_request_maker.h"
#include "aos/common/common.h"
#include "aos/common/exchange_id.h"
#include "aos/request/i_request.h"

namespace aoe {
namespace bybit {
namespace cancel_order {
template <template <typename> typename MemoryPool>
class RequestInterface : public aos::RequestInterface<MemoryPool> {
  public:
    ~RequestInterface() override = default;
    virtual common::ExchangeId ExchangeId() const { return exchange_id_; };
    virtual const aos::TradingPair& TradingPair() const {
        return trading_pair_;
    };
    virtual aoe::bybit::Category Category() const { return category_; };
    virtual uint64_t OrderId() const { return order_id_; };
    virtual void SetOrderId(uint64_t order_id) { order_id_ = order_id; };
    virtual void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    };
    virtual void SetCategory(aoe::bybit::Category category) {
        category_ = category;
    };
    // json appearance for bybit
    virtual std::pair<bool, nlohmann::json> Accept(
        aoe::bybit::cancel_order::RequestMakerInterface<MemoryPool>*
            request_maker) = 0;

  protected:
    common::ExchangeId exchange_id_ = common::ExchangeId::kBybit;
    aos::TradingPair trading_pair_;
    aoe::bybit::Category category_ = aoe::bybit::Category::kInvalid;
    uint64_t order_id_;
    aoe::bybit::OrderRouting order_routing_;
};
};  // namespace cancel_order
};  // namespace bybit
};  // namespace aoe