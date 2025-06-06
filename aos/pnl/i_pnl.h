#pragma once
#include "aos/trading_pair/trading_pair.h"
#include "aos/common/exchange_id.h"
namespace aos {
namespace impl {
template <typename Price, typename Qty>
class IRealizedPnl {
  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    virtual ~IRealizedPnl() {}
    virtual RealizedPnl GetRealizedPnl() = 0;
};

template <typename Price, typename Qty>
class IRealizedPnlForTradingPair {
  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());

    virtual ~IRealizedPnlForTradingPair() {}

    // Метод для получения реализованной прибыли для заданной пары
    virtual std::pair<bool, RealizedPnl> GetRealizedPnl(
        common::ExchangeId exchange, aos::TradingPair tradingPair) const = 0;
};
};  // namespace impl
};  // namespace aos