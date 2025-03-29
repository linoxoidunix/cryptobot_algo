#pragma once
#include "aos/common/ref_counted.h"
#include "aot/common/types.h"

namespace aos {

template <typename Qty, template <typename> class MemoryPool, typename Uid>
class IPositionStorageByOrderId
    : public common::RefCounted<
          MemoryPool, IPositionStorageByOrderId<Qty, MemoryPool, Uid>> {
  public:
    virtual ~IPositionStorageByOrderId()                    = default;

    // Получить позицию для определенной торговой пары на указанной бирже
    virtual std::pair<bool, Qty> GetPosition(common::ExchangeId exchange,
                                             common::TradingPair tradingPair,
                                             Uid uid) const = 0;

    virtual void AddPosition(common::ExchangeId exchange,
                             common::TradingPair& tradingPair, Qty qty,
                             Uid uid)                       = 0;

    virtual bool RemovePosition(common::ExchangeId exchange,
                                common::TradingPair& tradingPair, Qty qty,
                                Uid uid)                    = 0;
};

}  // namespace aos