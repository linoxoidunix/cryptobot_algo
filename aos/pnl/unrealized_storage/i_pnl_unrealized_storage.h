#pragma once
#include "aos/common/ref_counted.h"
#include "aos/trading_pair/trading_pair.h"
#include "aot/common/types.h"
namespace aos {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class IPnlUnRealizedStorage
    : public common::RefCounted<MemoryPool,
                                IPnlUnRealizedStorage<Price, Qty, MemoryPool>> {
  public:
    using UnRealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    virtual void UpdatePosition(common::ExchangeId exchange,
                                aos::TradingPair tradingPair, Price avg_price,
                                Qty avg_net_qty) = 0;
    virtual void UpdateBBO(common::ExchangeId exchange,
                           aos::TradingPair tradingPair, Price price_bid,
                           Price price_ask)      = 0;
    virtual std::pair<bool, UnRealizedPnl> GetUnRealizedPnl(
        common::ExchangeId exchange, aos::TradingPair tradingPair) const = 0;
};

template <typename Price, typename Qty>
class PnlUnRealizedStorageInterface {
  public:
    using UnRealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    virtual void UpdatePosition(common::ExchangeId exchange,
                                aos::TradingPair tradingPair, Price avg_price,
                                Qty avg_net_qty) = 0;
    virtual void UpdateBBO(common::ExchangeId exchange,
                           aos::TradingPair tradingPair, Price price_bid,
                           Price price_ask)      = 0;
    virtual std::pair<bool, UnRealizedPnl> GetUnRealizedPnl(
        common::ExchangeId exchange, aos::TradingPair tradingPair) const = 0;
};
};  // namespace aos