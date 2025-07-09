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
    explicit UserDataSubscriptionBuilder(WebSocketPrivateSessionRWInterface& ws)
        : ws_(ws) {}
    ~UserDataSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json request_json;
        request_json["id"]     = nullptr;
        request_json["method"] = {"userDataStream.subscribe"};
        ws_.AsyncWrite(std::move(request_json));
    }
};

class ExecutionSubscriptionBuilder : public SubscriptionBuilderInterface {
    UserDataSubscriptionBuilder user_data_subscription_builder_;

  public:
    explicit ExecutionSubscriptionBuilder(
        WebSocketPrivateSessionRWInterface& ws)
        : user_data_subscription_builder_(ws) {}
    ~ExecutionSubscriptionBuilder() override = default;
    void Subscribe() override { user_data_subscription_builder_.Subscribe(); }
};

class OrderSubscriptionBuilder : public SubscriptionBuilderInterface {
    UserDataSubscriptionBuilder user_data_subscription_builder_;

  public:
    explicit OrderSubscriptionBuilder(WebSocketPrivateSessionRWInterface& ws)
        : user_data_subscription_builder_(ws) {}
    ~OrderSubscriptionBuilder() override = default;
    void Subscribe() override { user_data_subscription_builder_.Subscribe(); }
};

namespace internal {

template <typename UpdateSpeedType>
class DiffSubscriptionBuilderBase : public SubscriptionBuilderInterface {
  protected:
    WebSocketPublicSessionRWInterface& ws_;
    UpdateSpeedType update_speed_;
    aos::TradingPair trading_pair_;

  public:
    DiffSubscriptionBuilderBase(WebSocketPublicSessionRWInterface& ws,
                                UpdateSpeedType update_speed,
                                aos::TradingPair trading_pair)
        : ws_(ws), update_speed_(update_speed), trading_pair_(trading_pair) {}

    ~DiffSubscriptionBuilderBase() override = default;

    void Subscribe() override {
        nlohmann::json request_json;
        request_json["id"]     = nullptr;
        request_json["method"] = "SUBSCRIBE";

        auto [status_trading_pair, result_trading_pair] =
            aos::impl::TradingPairToSmallStringView::Convert(trading_pair_);
        if (!status_trading_pair) return;

        auto [status_depth, result_speed_update] =
            DiffUpdateSpeed_ms_ToString(update_speed_);
        if (!status_depth) return;

        request_json["params"] = {fmt::format(
            "{}@depth@{}", result_trading_pair, result_speed_update)};
        ws_.AsyncWrite(std::move(request_json));
    }
};

}  // namespace internal

namespace spot {
using DiffSubscriptionBuilder = internal::DiffSubscriptionBuilderBase<
    aoe::binance::spot::DiffUpdateSpeed_ms>;
}  // namespace spot

namespace futures {
using DiffSubscriptionBuilder = internal::DiffSubscriptionBuilderBase<
    aoe::binance::futures::DiffUpdateSpeed_ms>;
}  // namespace futures

class TradeSubscriptionBuilderBase : public SubscriptionBuilderInterface {
  protected:
    WebSocketPublicSessionRWInterface& ws_;
    aos::TradingPair trading_pair_;

  public:
    TradeSubscriptionBuilderBase(WebSocketPublicSessionRWInterface& ws,
                                 aos::TradingPair trading_pair)
        : ws_(ws), trading_pair_(trading_pair) {}

    ~TradeSubscriptionBuilderBase() override = default;

    void Subscribe() override {
        nlohmann::json request_json;
        request_json["id"]     = nullptr;
        request_json["method"] = "SUBSCRIBE";

        auto [status_trading_pair, result_trading_pair] =
            aos::impl::TradingPairToSmallStringView::Convert(trading_pair_);
        if (!status_trading_pair) return;

        request_json["params"] = {fmt::format("{}@trade", result_trading_pair)};
        ws_.AsyncWrite(std::move(request_json));
    }
};

namespace spot {
using TradeSubscriptionBuilder = TradeSubscriptionBuilderBase;
}  // namespace spot

namespace futures {
using TradeSubscriptionBuilder = TradeSubscriptionBuilderBase;
}  // namespace futures
}  // namespace impl
};  // namespace binance
};  // namespace aoe
