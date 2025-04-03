#pragma once
#include "aos/common/ref_counted.h"
#include "aot/common/types.h"

namespace aos {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class IPnlRealizedStorage
    : public common::RefCounted<MemoryPool,
                                IPnlRealizedStorage<Price, Qty, MemoryPool>> {
  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    virtual void Add(common::ExchangeId exchange,
                     common::TradingPair tradingPair, RealizedPnl pnl) = 0;
    virtual std::pair<bool, RealizedPnl> GetRealizedPnl(
        common::ExchangeId exchange, common::TradingPair tradingPair) const = 0;
};
};  // namespace aos