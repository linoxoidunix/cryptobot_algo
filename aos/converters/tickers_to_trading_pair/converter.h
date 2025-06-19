#pragma once
#include <array>
#include <cstddef>

#include "aos/ticker/ticker.h"
#include "aos/trading_pair/trading_pair.h"
namespace aos {
constexpr std::array<std::pair<Ticker, Ticker>,
                     static_cast<size_t>(TradingPair::kCount)>
    kTradingPairMap = {{
        {Ticker::kBTC, Ticker::kUSDT},  // BTCUSDT
        {Ticker::kETH, Ticker::kUSDT},  // ETHUSDT
    }};
}

namespace aos {
constexpr std::pair<bool, std::pair<Ticker, Ticker>> TradingPairToTickers(
    TradingPair id) {
    constexpr std::pair<Ticker, Ticker> kInvalidPair{Ticker::kCount,
                                                     Ticker::kCount};
    const auto index = static_cast<size_t>(id);

    if (index < static_cast<size_t>(TradingPair::kCount)) {
        return {true, kTradingPairMap[index]};
    }
    return {false, kInvalidPair};
}
};  // namespace aos
