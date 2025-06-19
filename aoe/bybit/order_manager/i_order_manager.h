#pragma once
#include "aoe/bybit/order_mutator/i_order_mutator.h"
#include "aos/order_manager/i_order_manager.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class OrderManagerInterface : public aos::OrderManagerInterface<MemoryPool>,
                              public OrderMutatorInterface {
  public:
    ~OrderManagerInterface() override = default;
};
};  // namespace bybit
};  // namespace aoe