#pragma once
#include "aoe/binance/leaves_qty_calculator/i_leaves_qty_calculator.h"

namespace aoe {
namespace binance {
namespace impl {
template <template <typename> typename MemoryPool>
class LeavesQtyCalculator : public LeavesQtyCalculatorInterface<MemoryPool> {
  public:
    ~LeavesQtyCalculator() override = default;
    double Calculate(
        const OrderEventInterface<MemoryPool>& event) const override {
        return event.OrderQty() - event.CumExecQty();
    };
};
};  // namespace impl
};  // namespace binance
};  // namespace aoe
