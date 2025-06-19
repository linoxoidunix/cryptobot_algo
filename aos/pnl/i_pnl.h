#pragma once
#include "aos/common/exchange_id.h"
#include "aos/trading_pair/trading_pair.h"
namespace aos {
namespace impl {
template <typename Price, typename Qty>
class IRealizedPnl {
  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    virtual ~IRealizedPnl()              = default;
    virtual RealizedPnl GetRealizedPnl() = 0;
};

template <typename Price, typename Qty>
class IRealizedPnlForTradingPair {
  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());

    virtual ~IRealizedPnlForTradingPair() = default;

    // Метод для получения реализованной прибыли для заданной пары
    virtual std::pair<bool, RealizedPnl> GetRealizedPnl(
        common::ExchangeId exchange, aos::TradingPair tradingPair) const = 0;
};
};  // namespace impl
};  // namespace aos