#pragma once
#include "aos/om/i_om.h"

namespace aos {

template <template <typename> typename MemoryPool>
class OMDefault : public OMInterface<MemoryPool> {
  public:
    void PlaceOrder(
        boost::intrusive_ptr<OrderTypeInterface<MemoryPool>> order) override {};
    bool CancelOrder(common::ExchangeId exchange_id, OrderRouting routing,
                     uint64_t order_id) override {
        return true;
    };
    void OnResponse(boost::intrusive_ptr<OrderEventInterface<MemoryPool>>
                        orderevent) override {};
};
};  // namespace aos