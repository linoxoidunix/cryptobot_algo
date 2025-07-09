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
namespace linear {
class TickersSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPublicSessionRWInterface& ws_;
    aos::TradingPair trading_pair_;

  public:
    TickersSubscriptionBuilder(WebSocketPublicSessionRWInterface& ws,
                               aos::TradingPair trading_pair)
        : ws_(ws), trading_pair_(trading_pair) {}
    ~TickersSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json request_json;
        request_json["op"] = "subscribe";
        auto [status_trading_pair, result_trading_pair] =
            aos::impl::TradingPairToBigStringView::Convert(trading_pair_);
        if (!status_trading_pair) return;
        request_json["args"] = {fmt::format("tickers.{}", result_trading_pair)};
        ws_.AsyncWrite(std::move(request_json));
    }
};
};  // namespace linear
};  // namespace impl
};  // namespace bybit
};  // namespace aoe
