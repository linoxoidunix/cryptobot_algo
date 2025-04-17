#pragma once
#include "aos/order_event/i_order_event.h"
#include "aos/order_manager/i_order_manager.h"
#include "aos/order_types/i_order_type.h"
#include "aot/common/mem_pool.h"
#include "boost/intrusive_ptr.hpp"

namespace aos {

template <template <typename> typename MemoryPool>
class MultiOrderManagerInterface {
  public:
    virtual ~MultiOrderManagerInterface() = default;

    virtual void Register(
        common::ExchangeId,
        std::unique_ptr<OrderManagerInterface<MemoryPool>>) = 0;

    virtual void PlaceOrder(
        common::ExchangeId,
        boost::intrusive_ptr<OrderTypeInterface<MemoryPool>> order) = 0;

    virtual void OnResponse(
        common::ExchangeId id,
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) = 0;
};
};  // namespace aos
