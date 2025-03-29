#pragma once

#include "aos/common/ref_counted.h"
#include "aos/order_manager/order_manager_types.h"
#include "aot/common/types.h"

namespace aos {
template <typename Price, typename Qty, typename Uid>
class IOrderSubmitterNoPool {
  public:
    virtual void SubmitOrder(common::ExchangeId exchange_id,
                             common::TradingPair trading_pair, Price price,
                             Qty qty, common::Side side, TimeInForce tif,
                             OrderRouting routing, Uid uid) = 0;

    virtual ~IOrderSubmitterNoPool()                        = default;
};

template <typename Uid>
class IOrderSubmiCancelNoPool {
  public:
    virtual void SubmitCancelOrder(common::ExchangeId exchange_id,
                                   OrderRouting routing, Uid uid) = 0;

    virtual ~IOrderSubmiCancelNoPool()                            = default;
};

template <typename Price, typename Qty, template <typename> class MemoryPool,
          typename Uid>
class IOrderSubmitter
    : public IOrderSubmitterNoPool<Price, Qty, Uid>,
      public IOrderSubmiCancelNoPool<Uid>,
      public common::RefCounted<
          MemoryPool, aos::IOrderSubmitter<Price, Qty, MemoryPool, Uid>> {
  public:
    virtual ~IOrderSubmitter() = default;
};
};  // namespace aos