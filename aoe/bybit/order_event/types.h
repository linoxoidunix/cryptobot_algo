#pragma once
#include "aoe/bybit/order_event/i_types.h"
#include "aos/order_manager/order_manager/i_order_manager.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class OrderEventDefault : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventDefault() = default;
    ~OrderEventDefault() override {};
    void Accept(aos::PositionStorageByPairInterface<double, double, PositionT>&
                    position_storage) override {};
};

};  // namespace bybit
};  // namespace aoe