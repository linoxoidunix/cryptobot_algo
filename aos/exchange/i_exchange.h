#pragma once
#include "aos/order_manager/order_manager/i_order_manager.h"
#include "aot/common/types.h"

namespace aos {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class IExchange
    : public common::RefCounted<MemoryPool,
                                IExchange<Price, Qty, MemoryPool, Uid>>,
      public IOrderUpdateManager<Price, Qty, MemoryPool, Uid> {
  public:
    virtual void SubmitOrder(common::TradingPair trading_pair, Price price,
                             Qty qty, common::Side side, TimeInForce tif,
                             Uid uid) = 0;
    virtual void CancelOrder(Uid uid) = 0;
    virtual ~IExchange()              = default;
};
};  // namespace aos