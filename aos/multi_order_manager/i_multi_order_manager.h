#pragma once
#include "aos/order_event/i_order_event.h"
#include "aos/order_manager/i_order_manager.h"
#include "aos/request/i_request.h"
#include "aos/common/mem_pool.h"
#include "boost/intrusive_ptr.hpp"

namespace aos {

template <template <typename> typename MemoryPool>
class MultiOrderManagerInterface {
  public:
    virtual ~MultiOrderManagerInterface() = default;

    virtual void Register(
        common::ExchangeId,
        std::unique_ptr<OrderManagerInterface<MemoryPool>>) = 0;

    virtual void PlaceOrderManualId(
        common::ExchangeId,
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order,
        uint64_t uid) = 0;

    virtual void PlaceOrderAutoId(
        common::ExchangeId,
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order) = 0;

    virtual void CancelOrder(
        common::ExchangeId,
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order) = 0;

    virtual void AmendOrder(
        common::ExchangeId,
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order) = 0;

    virtual void OnResponse(
        common::ExchangeId id,
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) = 0;
};
};  // namespace aos
