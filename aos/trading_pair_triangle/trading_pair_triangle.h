#pragma once
#include <array>
#include <cstdint>

#include "aos/converters/tickers_to_trading_pair/converter.h"
#include "aos/market_type/market_type.h"
#include "aos/trading_pair/trading_pair.h"

namespace aos {

enum class ActionType : uint8_t { kBuy, kSell };

struct Action {
    ActionType action_type;
    MarketType market_type;
    TradingPair trading_pair;
};

namespace spot {
inline Action Buy(TradingPair trading_pair) {
    return Action{ActionType::kBuy, MarketType::kSpot, trading_pair};
}

inline Action Sell(TradingPair trading_pair) {
    return Action{ActionType::kSell, MarketType::kSpot, trading_pair};
}
}  // namespace spot

struct Triangle {
    std::array<Action, 3> steps;

    Triangle(const Action& a1, const Action& a2, const Action& a3)
        : steps{a1, a2, a3} {}
};

// Удобная фабричная функция
template <typename A1, typename A2, typename A3>
inline Triangle MakeTriangle(A1&& a1, A2&& a2, A3&& a3) {
    return Triangle{std::forward<A1>(a1), std::forward<A2>(a2),
                    std::forward<A3>(a3)};
}
};  // namespace aos

template <>
class fmt::formatter<aos::Action> {
  public:
    static constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }
    template <typename Context>
    constexpr auto format(const aos::Action& foo, Context& ctx) const {
        auto action_type = foo.action_type;
        std::string action_str =
            (action_type == aos::ActionType::kBuy ? "BUY" : "SELL");
        return fmt::format_to(ctx.out(), "{} {} on {}", action_str,
                              foo.trading_pair, foo.market_type);
    }
};

template <>
class fmt::formatter<aos::Triangle> {
  public:
    static constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }
    template <typename Context>
    constexpr auto format(const aos::Triangle& foo, Context& ctx) const {
        return fmt::format_to(ctx.out(), "Triangle: {} -> {} -> {}",
                              foo.steps[0], foo.steps[1], foo.steps[2]);
    }
};