#pragma once
#include "aot/common/types.h"

namespace aos {

template <typename Price, typename Qty>
class PositionTrackerInterface {
  public:
    virtual void AddPosition(common::ExchangeId exchange,
                             common::TradingPair assetPair, Price price,
                             Qty qty)    = 0;
    virtual bool RemovePosition(common::ExchangeId exchange,
                                common::TradingPair assetPair, Price price,
                                Qty qty) = 0;

    // virtual void UpdatePosition(const common::TradingPair& pair,
    //                                          Qty qty)              = 0;
    // virtual Qty GetPosition(const common::TradingPair& pair) const = 0;
    virtual ~PositionTrackerInterface()  = default;
};
template <typename Price, typename Qty, template <typename> class MemoryPool>
class IPositionTracker
    : public common::RefCounted<MemoryPool,
                                IPositionTracker<Price, Qty, MemoryPool>> {
  public:
    virtual void AddPosition(common::ExchangeId exchange,
                             common::TradingPair assetPair, Price price,
                             Qty qty)    = 0;
    virtual bool RemovePosition(common::ExchangeId exchange,
                                common::TradingPair assetPair, Price price,
                                Qty qty) = 0;

    // virtual void UpdatePosition(const common::TradingPair& pair,
    //                                          Qty qty)              = 0;
    // virtual Qty GetPosition(const common::TradingPair& pair) const = 0;
    virtual ~IPositionTracker()          = default;
};
};  // namespace aos