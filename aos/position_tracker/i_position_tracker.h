#pragma once
#include "aos/common/exchange_id.h"
#include "aos/trading_pair/trading_pair.h"
namespace aos {

template <typename Price, typename Qty>
class PositionTrackerInterface {
  public:
    virtual void AddPosition(common::ExchangeId exchange,
                             aos::TradingPair assetPair, Price price,
                             Qty qty)    = 0;
    virtual bool RemovePosition(common::ExchangeId exchange,
                                aos::TradingPair assetPair, Price price,
                                Qty qty) = 0;

    // virtual void UpdatePosition(const aos::TradingPair& pair,
    //                                          Qty qty)              = 0;
    // virtual Qty GetPosition(const aos::TradingPair& pair) const = 0;
    virtual ~PositionTrackerInterface()  = default;
};
template <typename Price, typename Qty, template <typename> class MemoryPool>
class IPositionTracker
    : public common::RefCounted<MemoryPool,
                                IPositionTracker<Price, Qty, MemoryPool>> {
  public:
    virtual void AddPosition(common::ExchangeId exchange,
                             aos::TradingPair assetPair, Price price,
                             Qty qty)    = 0;
    virtual bool RemovePosition(common::ExchangeId exchange,
                                aos::TradingPair assetPair, Price price,
                                Qty qty) = 0;

    // virtual void UpdatePosition(const aos::TradingPair& pair,
    //                                          Qty qty)              = 0;
    // virtual Qty GetPosition(const aos::TradingPair& pair) const = 0;
    ~IPositionTracker() override         = default;
};
};  // namespace aos