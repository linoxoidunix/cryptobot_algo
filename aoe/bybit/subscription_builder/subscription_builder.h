#pragma once
#include "aoe/bybit/enum_printer/enum_printer.h"
#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/session/web_socket/i_web_socket.h"
#include "aoe/session/web_socket/i_web_socket.h"
#include "aoe/subscription_builder/i_subscription_builder.h"
#include "aos/trading_pair_printer/i_trading_pair_printer.h"
namespace aoe {
namespace bybit {
namespace impl {
class ExecutionSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPrivateSessionRWInterface& ws_;

  public:
    ExecutionSubscriptionBuilder(WebSocketPrivateSessionRWInterface& ws)
        : ws_(ws) {}
    ~ExecutionSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json j_;
        j_["op"]   = "subscribe";
        j_["args"] = {"execution"};
        ws_.AsyncWrite(std::move(j_));
    }
};
class OrderSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPrivateSessionRWInterface& ws_;

  public:
    OrderSubscriptionBuilder(WebSocketPrivateSessionRWInterface& ws)
        : ws_(ws) {}
    ~OrderSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json j_;
        j_["op"]   = "subscribe";
        j_["args"] = {"order"};
        ws_.AsyncWrite(std::move(j_));
    }
};

namespace spot {
class OrderBookSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPublicSessionRWInterface& ws_;
    aoe::bybit::spot::Depth depth_;
    common::TradingPair trading_pair_;
    aos::TradingPairPrinterInterface& printer_;

  public:
    OrderBookSubscriptionBuilder(WebSocketPublicSessionRWInterface& ws,
                                 aoe::bybit::spot::Depth depth,
                                 common::TradingPair trading_pair,
                                 aos::TradingPairPrinterInterface& printer)
        : ws_(ws),
          depth_(depth),
          trading_pair_(trading_pair),
          printer_(printer) {}
    ~OrderBookSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json j_;
        j_["op"] = "subscribe";
        auto [status_trading_pair, result_trading_pair] =
            printer_.Print(trading_pair_);
        if (!status_trading_pair) return;
        auto [status_depth, result_depth] = DepthToString(depth_);
        if (!status_depth) return;
        j_["args"] = {
            fmt::format("orderbook.{}.{}", result_depth, result_trading_pair)};
        ws_.AsyncWrite(std::move(j_));
    }
};
};  // namespace spot
};  // namespace impl
};  // namespace bybit
};  // namespace aoe
