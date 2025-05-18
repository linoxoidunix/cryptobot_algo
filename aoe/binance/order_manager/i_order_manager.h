#pragma once
#include "aoe/binance/order_mutator/i_order_mutator.h"
#include "aos/order_manager/i_order_manager.h"

namespace aoe {
namespace binance {
template <template <typename> typename MemoryPool>
class OrderManagerInterface : public aos::OrderManagerInterface<MemoryPool>,
                              public OrderMutatorInterface {
  public:
    virtual ~OrderManagerInterface() = default;
};
};  // namespace binance
};  // namespace aoe