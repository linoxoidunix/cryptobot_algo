#pragma once
#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/order_event/i_types.h"
#include "aos/order_types/i_order_type.h"

namespace aos {

template <template <typename> typename MemoryPool>
class OMInterface {
  public:
    virtual ~OMInterface() = default;
    virtual void PlaceOrder(
        boost::intrusive_ptr<OrderTypeInterface<MemoryPool>> order)   = 0;
    virtual bool CancelOrder(common::ExchangeId exchange_id,
                             OrderRouting routing, uint64_t order_id) = 0;
    virtual void OnResponse(
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> orderevent) = 0;
};
};  // namespace aos