#pragma once
#include "aos/order_event/i_order_event.h"
#include "aos/order_types/i_order_type.h"

namespace aos {
template <template <typename> typename MemoryPool>
class OrderManagerInterface {
  public:
    virtual ~OrderManagerInterface() = default;

    virtual void PlaceOrder(
        boost::intrusive_ptr<OrderTypeInterface<MemoryPool>> order) = 0;

    virtual void OnResponse(
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) = 0;
};
};  // namespace aos