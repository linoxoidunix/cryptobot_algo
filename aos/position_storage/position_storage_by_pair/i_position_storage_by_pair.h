#pragma once
#include "aot/common/types.h"

namespace aos {

template <typename Qty>
class IPositionStorageByPair {
  public:
    virtual ~IPositionStorageByPair() = default;

    // Получить позицию для определенной торговой пары на указанной бирже
    virtual std::pair<bool, Qty> GetPosition(
        common::ExchangeId exchange, common::TradingPair tradingPair) const = 0;

    virtual void AddPosition(common::ExchangeId exchange,
                             common::TradingPair& tradingPair, Qty qty)     = 0;

    virtual bool RemovePosition(common::ExchangeId exchange,
                                common::TradingPair& tradingPair, Qty qty)  = 0;
};

}  // namespace aos