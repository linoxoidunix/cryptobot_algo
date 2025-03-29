#pragma once
#include "aos/order_manager/order_manager_types.h"
#include "aot/common/types.h"
#include "boost/intrusive_ptr.hpp"

namespace aos {

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class IExchange;

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class IExchangeRegistry
    : public common::RefCounted<
          MemoryPool, IExchangeRegistry<Price, Qty, MemoryPool, Uid>> {
  public:
    virtual void RegisterExchange(
        OrderRouting routing, common::ExchangeId exchange_id,
        boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>>
            exchange) = 0;

    virtual std::pair<
        bool, boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>>>
    GetExchange(OrderRouting routing, common::ExchangeId exchange_id) const = 0;

    virtual ~IExchangeRegistry() = default;
};
};  // namespace aos