#pragma once
#include "aos/common/exchange_id.h"
#include "aos/common/ref_counted.h"
#include "aos/position_tracker/i_position_tracker.h"
#include "aos/trading_pair/trading_pair.h"
namespace aos {

template <typename Price, typename Qty, typename PositionT>
class PositionStorageByPairInterface
    : public PositionTrackerInterface<Price, Qty> {
  public:
    ~PositionStorageByPairInterface() override = default;

    // Получить позицию для определенной торговой пары на указанной бирже
    virtual std::optional<std::reference_wrapper<const PositionT>> GetPosition(
        common::ExchangeId exchange, aos::TradingPair tradingPair) const = 0;
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class IPositionStorageByPair : public IPositionTracker<Price, Qty, MemoryPool> {
  public:
    ~IPositionStorageByPair() override = default;

    // Получить позицию для определенной торговой пары на указанной бирже
    virtual std::pair<bool, Qty> GetPosition(
        common::ExchangeId exchange, aos::TradingPair tradingPair) const = 0;
};

}  // namespace aos