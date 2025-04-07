#pragma once
#include "aos/common/ref_counted.h"
#include "aos/position_tracker/i_position_tracker.h"
#include "aot/common/types.h"
namespace aos {

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class IPositionStorageByPair : public IPositionTracker<Price, Qty, MemoryPool> {
  public:
    virtual ~IPositionStorageByPair() = default;

    // Получить позицию для определенной торговой пары на указанной бирже
    virtual std::pair<bool, Qty> GetPosition(
        common::ExchangeId exchange, common::TradingPair tradingPair) const = 0;
};

}  // namespace aos