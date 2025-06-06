#pragma once
#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/request_maker/for_web_socket/amend_order/i_request_maker.h"
#include "aos/common/common.h"
#include "aos/request/i_request.h"
#include "aos/trading_pair/trading_pair.h"
#include "aos/common/exchange_id.h"
namespace aoe {
namespace bybit {
namespace amend_order {
template <template <typename> typename MemoryPool>
class RequestInterface : public aos::RequestInterface<MemoryPool> {
  public:
    virtual ~RequestInterface() = default;
    virtual common::ExchangeId ExchangeId() const { return exchange_id_; };
    virtual const aos::TradingPair& TradingPair() const {
        return trading_pair_;
    };
    virtual aoe::bybit::Category Category() const { return category_; };
    virtual uint64_t OrderId() const { return order_id_; };
    // json appearance for bybit
    virtual std::pair<bool, nlohmann::json> Accept(
        aoe::bybit::amend_order::RequestMakerInterface<MemoryPool>*
            request_maker) = 0;

  protected:
    common::ExchangeId exchange_id_ = common::ExchangeId::kBybit;
    aos::TradingPair trading_pair_;
    aoe::bybit::Category category_ = aoe::bybit::Category::kInvalid;
    uint64_t order_id_;
    aoe::bybit::OrderRouting order_routing_;
};
};  // namespace amend_order
};  // namespace bybit
};  // namespace aoe