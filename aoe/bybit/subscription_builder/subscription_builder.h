#pragma once
#include "aoe/bybit/enum_printer/enum_printer.h"
#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/session/web_socket/i_web_socket.h"
#include "aoe/session/web_socket/i_web_socket.h"
#include "aoe/subscription_builder/i_subscription_builder.h"
#include "aos/converters/trading_pair_to_big_string_view/trading_pair_to_big_string_view.h"
#include "aos/trading_pair/trading_pair.h"
namespace aoe {
namespace bybit {
namespace impl {
class ExecutionSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPrivateSessionRWInterface& ws_;

  public:
    explicit ExecutionSubscriptionBuilder(
        WebSocketPrivateSessionRWInterface& ws)
        : ws_(ws) {}
    ~ExecutionSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json request_json;
        request_json["op"]   = "subscribe";
        request_json["args"] = {"execution"};
        ws_.AsyncWrite(std::move(request_json));
    }
};
class OrderSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPrivateSessionRWInterface& ws_;

  public:
    explicit OrderSubscriptionBuilder(WebSocketPrivateSessionRWInterface& ws)
        : ws_(ws) {}
    ~OrderSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json request_json;
        request_json["op"]   = "subscribe";
        request_json["args"] = {"order"};
        ws_.AsyncWrite(std::move(request_json));
    }
};

namespace spot {
class OrderBookSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPublicSessionRWInterface& ws_;
    aoe::bybit::spot::Depth depth_;
    aos::TradingPair trading_pair_;

  public:
    OrderBookSubscriptionBuilder(WebSocketPublicSessionRWInterface& ws,
                                 aoe::bybit::spot::Depth depth,
                                 aos::TradingPair trading_pair)
        : ws_(ws), depth_(depth), trading_pair_(trading_pair) {}
    ~OrderBookSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json request_json;
        request_json["op"] = "subscribe";
        auto [status_trading_pair, result_trading_pair] =
            aos::impl::TradingPairToBigStringView::Convert(trading_pair_);
        if (!status_trading_pair) return;
        auto [status_depth, result_depth] = DepthToString(depth_);
        if (!status_depth) return;
        request_json["args"] = {
            fmt::format("orderbook.{}.{}", result_depth, result_trading_pair)};
        ws_.AsyncWrite(std::move(request_json));
    }
};
};  // namespace spot
namespace linear {
class OrderBookSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPublicSessionRWInterface& ws_;
    aoe::bybit::linear::Depth depth_;
    aos::TradingPair trading_pair_;

  public:
    OrderBookSubscriptionBuilder(WebSocketPublicSessionRWInterface& ws,
                                 aoe::bybit::linear::Depth depth,
                                 aos::TradingPair trading_pair)
        : ws_(ws), depth_(depth), trading_pair_(trading_pair) {}
    ~OrderBookSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json request_json;
        request_json["op"] = "subscribe";
        auto [status_trading_pair, result_trading_pair] =
            aos::impl::TradingPairToBigStringView::Convert(trading_pair_);
        if (!status_trading_pair) return;
        auto [status_depth, result_depth] = DepthToString(depth_);
        if (!status_depth) return;
        request_json["args"] = {
            fmt::format("orderbook.{}.{}", result_depth, result_trading_pair)};
        ws_.AsyncWrite(std::move(request_json));
    }
};
};  // namespace linear
};  // namespace impl
};  // namespace bybit
};  // namespace aoe
