#pragma once
#include "aoe/binance/enum_printer/enum_printer.h"
#include "aoe/binance/enums/enums.h"
#include "aoe/binance/session/web_socket/i_web_socket.h"
#include "aoe/session/web_socket/i_web_socket.h"
#include "aoe/subscription_builder/i_subscription_builder.h"
#include "aos/converters/trading_pair_to_small_string_view/trading_pair_to_small_string_view.h"
#include "aos/trading_pair/trading_pair.h"
namespace aoe {
namespace binance {
namespace impl {

class UserDataSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPrivateSessionRWInterface& ws_;

  public:
    UserDataSubscriptionBuilder(WebSocketPrivateSessionRWInterface& ws)
        : ws_(ws) {}
    ~UserDataSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json j_;
        j_["id"]     = nullptr;
        j_["method"] = {"userDataStream.subscribe"};
        ws_.AsyncWrite(std::move(j_));
    }
};

class ExecutionSubscriptionBuilder : public SubscriptionBuilderInterface {
    UserDataSubscriptionBuilder user_data_subscription_builder_;

  public:
    ExecutionSubscriptionBuilder(WebSocketPrivateSessionRWInterface& ws)
        : user_data_subscription_builder_(ws) {}
    ~ExecutionSubscriptionBuilder() override = default;
    void Subscribe() override { user_data_subscription_builder_.Subscribe(); }
};

class OrderSubscriptionBuilder : public SubscriptionBuilderInterface {
    UserDataSubscriptionBuilder user_data_subscription_builder_;

  public:
    OrderSubscriptionBuilder(WebSocketPrivateSessionRWInterface& ws)
        : user_data_subscription_builder_(ws) {}
    ~OrderSubscriptionBuilder() override = default;
    void Subscribe() override { user_data_subscription_builder_.Subscribe(); }
};

namespace spot {
class DiffSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPublicSessionRWInterface& ws_;
    aoe::binance::spot::DiffUpdateSpeed_ms update_speed_;
    aos::TradingPair trading_pair_;
    aos::impl::TradingPairToSmallStringView printer_;

  public:
    DiffSubscriptionBuilder(WebSocketPublicSessionRWInterface& ws,
                            aoe::binance::spot::DiffUpdateSpeed_ms update_speed,
                            aos::TradingPair trading_pair)
        : ws_(ws), update_speed_(update_speed), trading_pair_(trading_pair) {}
    ~DiffSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json j_;
        j_["id"]     = nullptr;
        j_["method"] = "SUBSCRIBE";
        auto [status_trading_pair, result_trading_pair] =
            printer_.Convert(trading_pair_);
        if (!status_trading_pair) return;
        auto [status_depth, result_speed_update] =
            DiffUpdateSpeed_ms_ToString(update_speed_);
        if (!status_depth) return;
        j_["params"] = {fmt::format("{}@depth@{}", result_trading_pair,
                                    result_speed_update)};
        ws_.AsyncWrite(std::move(j_));
    }
};
};  // namespace spot
namespace futures {
class DiffSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPublicSessionRWInterface& ws_;
    aoe::binance::futures::DiffUpdateSpeed_ms update_speed_;
    aos::TradingPair trading_pair_;
    aos::impl::TradingPairToSmallStringView printer_;

  public:
    DiffSubscriptionBuilder(
        WebSocketPublicSessionRWInterface& ws,
        aoe::binance::futures::DiffUpdateSpeed_ms update_speed,
        aos::TradingPair trading_pair)
        : ws_(ws), update_speed_(update_speed), trading_pair_(trading_pair) {}
    ~DiffSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json j_;
        j_["id"]     = nullptr;
        j_["method"] = "SUBSCRIBE";
        auto [status_trading_pair, result_trading_pair] =
            printer_.Convert(trading_pair_);
        if (!status_trading_pair) return;
        auto [status_depth, result_speed_update] =
            DiffUpdateSpeed_ms_ToString(update_speed_);
        if (!status_depth) return;
        j_["params"] = {fmt::format("{}@depth@{}", result_trading_pair,
                                    result_speed_update)};
        ws_.AsyncWrite(std::move(j_));
    }
};
};  // namespace futures
};  // namespace impl
};  // namespace binance
};  // namespace aoe
