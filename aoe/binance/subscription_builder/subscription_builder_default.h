#pragma once
#include "aoe/binance/subscription_builder/subscription_builder.h"
namespace aoe {
namespace binance {
namespace impl {
namespace main_net {
namespace spot {
class OrderBookSubscriptionBuilder : public SubscriptionBuilderInterface {
    aoe::binance::impl::spot::DiffSubscriptionBuilder
        diff_subscription_builder_;

  public:
    OrderBookSubscriptionBuilder(
        WebSocketPublicSessionRWInterface& ws,
        aoe::binance::spot::DiffUpdateSpeed_ms update_speed,
        aos::TradingPair trading_pair)
        : diff_subscription_builder_(ws, update_speed, trading_pair) {}
    ~OrderBookSubscriptionBuilder() override = default;
    void Subscribe() override { diff_subscription_builder_.Subscribe(); }
};
};  // namespace spot
};  // namespace main_net

};  // namespace impl
};  // namespace binance
};  // namespace aoe
