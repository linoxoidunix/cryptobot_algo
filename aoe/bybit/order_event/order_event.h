#pragma once
#include "aos/order_event/i_order_event.h"
#include "aos/position_storage/position_storage_by_pair/i_position_storage_by_pair.h"

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