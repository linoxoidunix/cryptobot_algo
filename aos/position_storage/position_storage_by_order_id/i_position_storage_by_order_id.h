#pragma once
#include "aos/common/ref_counted.h"
#include "aos/trading_pair/trading_pair.h"
#include "aos/common/exchange_id.h"
namespace aos {

class PositionStorageByOrderIdInterface {
  public:
    virtual ~PositionStorageByOrderIdInterface() = default;

    // Получить позицию для определенной торговой пары на указанной бирже
    virtual std::pair<bool, double> GetPosition(common::ExchangeId exchange,
                                                aos::TradingPair tradingPair,
                                                std::size_t uid) const = 0;

    virtual void AddPosition(common::ExchangeId exchange,
                             aos::TradingPair& tradingPair, double qty,
                             std::size_t uid)                          = 0;

    virtual bool RemovePosition(common::ExchangeId exchange,
                                aos::TradingPair& tradingPair, double qty,
                                std::size_t uid)                       = 0;
};

template <typename Qty, template <typename> class MemoryPool, typename Uid>
class IPositionStorageByOrderId
    : public common::RefCounted<
          MemoryPool, IPositionStorageByOrderId<Qty, MemoryPool, Uid>> {
  public:
    virtual ~IPositionStorageByOrderId()                    = default;

    // Получить позицию для определенной торговой пары на указанной бирже
    virtual std::pair<bool, Qty> GetPosition(common::ExchangeId exchange,
                                             aos::TradingPair tradingPair,
                                             Uid uid) const = 0;

    virtual void AddPosition(common::ExchangeId exchange,
                             aos::TradingPair& tradingPair, Qty qty,
                             Uid uid)                       = 0;

    virtual bool RemovePosition(common::ExchangeId exchange,
                                aos::TradingPair& tradingPair, Qty qty,
                                Uid uid)                    = 0;
};

}  // namespace aos