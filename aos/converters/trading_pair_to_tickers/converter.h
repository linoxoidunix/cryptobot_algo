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
        {Ticker::kETH, Ticker::kBTC},   // ETHBTC
        {Ticker::kBNB, Ticker::kUSDT},  // BNBUSDT
        {Ticker::kBNB, Ticker::kETH},   // BNBETH
        {Ticker::kSOL, Ticker::kUSDT},  // SOLUSDT
        {Ticker::kSOL, Ticker::kBNB},   // SOLBNB
        {Ticker::kXRP, Ticker::kUSDT},  // XRPUSDT
        {Ticker::kADA, Ticker::kUSDT},  // ADAUSDT
        {Ticker::kADA, Ticker::kXRP},   // ADAXRP
    }};
};

namespace aos {
constexpr std::pair<bool, std::pair<Ticker, Ticker>> TradingPairToTickers(
    TradingPair id) {
    using T = std::underlying_type_t<TradingPair>;
    constexpr std::pair<Ticker, Ticker> kInvalidPair{Ticker::kCount,
                                                     Ticker::kCount};
    const auto index = static_cast<T>(id);

    if (index < static_cast<T>(TradingPair::kCount)) {
        return {true, kTradingPairMap[index]};
    }
    return {false, kInvalidPair};
}
};  // namespace aos
