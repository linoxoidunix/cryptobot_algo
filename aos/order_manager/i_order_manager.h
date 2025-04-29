#pragma once
#include <cstdint>

#include "aos/order_event/i_order_event.h"
#include "aos/request/i_request.h"

namespace aos {
template <template <typename> typename MemoryPool>
class OrderManagerInterface {
  public:
    virtual ~OrderManagerInterface() = default;

    virtual void PlaceOrderManualId(
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order,
        uint64_t uid) = 0;
    virtual void PlaceOrderAutoId(
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order) = 0;
    virtual void CancelOrder(
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order) = 0;
    virtual void AmendOrder(
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order) = 0;
    virtual void OnResponse(
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) = 0;
};
};  // namespace aos