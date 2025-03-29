#pragma once
#include <deque>

#include "aos/common/ref_counted.h"
#include "aos/order_manager/order_manager_types.h"
#include "aot/common/types.h"

namespace aos {
template <typename Price, typename Qty, typename Uid>
class IOrderUpdaterNoPool {
  public:
    virtual void UpdateStatusOrder(Uid order_id, OrderStatus order_status) = 0;
    virtual ~IOrderUpdaterNoPool() = default;
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class IOrderUpdater
    : public IOrderUpdaterNoPool<Price, Qty, Uid>,
      public common::RefCounted<MemoryPool,
                                IOrderUpdater<Price, Qty, MemoryPool, Uid>> {
  public:
    virtual ~IOrderUpdater() = default;
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class IOrderUpdateManager {
  public:
    virtual void Subscribe(
        boost::intrusive_ptr<IOrderUpdater<Price, Qty, MemoryPool, Uid>>
            ptr)                   = 0;
    virtual ~IOrderUpdateManager() = default;
};

template <typename Uid>
class IOrderCanceller {
  public:
    virtual bool CancelOrder(common::ExchangeId exchange_id,
                             OrderRouting routing, Uid order_id) = 0;
    virtual ~IOrderCanceller()                                   = default;
};

// Интерфейс для управления ордерами (обновление и отмена)
template <typename Price, typename Qty, template <typename> class MemoryPool,
          typename Uid>
class IOrderManager : public IOrderUpdater<Price, Qty, MemoryPool, Uid>,
                      public IOrderCanceller<Uid> {
  public:
    virtual bool PlaceOrder(common::ExchangeId exchange_id,
                            common::TradingPair trading_pair, Price price,
                            Qty qty, common::Side side, TimeInForce tif,
                            OrderRouting routing) = 0;
    virtual ~IOrderManager()                      = default;
};
};  // namespace aos