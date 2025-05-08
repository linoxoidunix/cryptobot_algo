#pragma once
#include "aos/common/ref_counted.h"
#include "aos/trading_pair/trading_pair.h"
#include "aot/common/types.h"

namespace aos {
template <typename Price, typename Qty>
class PnlRealizedStorageInterface {
  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    virtual RealizedPnl FixProfit(common::ExchangeId exchange,
                                  aos::TradingPair tradingPair, Price avg_price,
                                  Qty net_qty, Price price, Qty qty) = 0;
    virtual std::pair<bool, RealizedPnl> GetRealizedPnl(
        common::ExchangeId exchange, aos::TradingPair tradingPair) const = 0;
    virtual ~PnlRealizedStorageInterface() = default;
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class IPnlRealizedStorage
    : public common::RefCounted<MemoryPool,
                                IPnlRealizedStorage<Price, Qty, MemoryPool>> {
  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    virtual void Add(common::ExchangeId exchange, aos::TradingPair tradingPair,
                     RealizedPnl pnl) = 0;
    virtual std::pair<bool, RealizedPnl> GetRealizedPnl(
        common::ExchangeId exchange, aos::TradingPair tradingPair) const = 0;
};
};  // namespace aos