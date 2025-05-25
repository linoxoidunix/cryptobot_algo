#pragma once

#include "aoe/binance/order_event/i_order_event.h"
namespace aoe {
namespace binance {
template <template <typename> typename MemoryPool>
class LeavesQtyCalculatorInterface {
  public:
    virtual ~LeavesQtyCalculatorInterface() = default;
    virtual double Calculate(const OrderEventInterface<MemoryPool>&) const = 0;
};
};  // namespace binance
};  // namespace aoe
