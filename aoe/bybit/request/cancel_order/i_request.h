#pragma once
#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/request_maker/for_web_socket/cancel_order/i_cancel_order.h"
#include "aos/common/common.h"
#include "aos/order_types/i_order_type.h"
#include "aot/common/types.h"

namespace aoe {
namespace bybit {
namespace cancel_order {
template <template <typename> typename MemoryPool>
class RequestInterface : public aos::OrderTypeInterface<MemoryPool> {
  public:
    virtual ~RequestInterface() = default;
    virtual common::ExchangeId ExchangeId() const { return exchange_id_; };
    virtual const common::TradingPair& TradingPair() const {
        return trading_pair_;
    };
    virtual aoe::bybit::Category Category() const { return category_; };
    virtual uint64_t OrderId() const { return order_id_; };
    // json appearance for bybit
    virtual std::pair<bool, nlohmann::json> Accept(
        aoe::bybit::cancel_order::RequestMakerInterface<MemoryPool>*
            request_maker) = 0;

  protected:
    common::ExchangeId exchange_id_ = common::ExchangeId::kBybit;
    common::TradingPair trading_pair_;
    aoe::bybit::Category category_ = aoe::bybit::Category::kInvalid;
    uint64_t order_id_;
    aoe::bybit::OrderRouting order_routing_;
};
};  // namespace cancel_order
};  // namespace bybit
};  // namespace aoe