#pragma once

#include <array>
#include <cstddef>

#include "aos/ticker/ticker.h"
#include "aos/trading_pair/trading_pair.h"

namespace aos {
// Таблица соответствия: [base][quote] -> TradingPair
constexpr std::size_t kTickerCount = static_cast<std::size_t>(Ticker::kCount);

// Прямое преобразование двумерного индекса [base][quote] в одномерный индекс
constexpr std::size_t FlatIndex(Ticker base, Ticker quote) {
    return static_cast<std::size_t>(base) * kTickerCount +
           static_cast<std::size_t>(quote);
}

// Плоский массив размером COUNT * COUNT
constexpr std::array<TradingPair, kTickerCount * kTickerCount> kReverseMapFlat =
    [] {
        std::array<TradingPair, kTickerCount * kTickerCount> map{};

        for (auto& e : map) {
            e = TradingPair::kCount;  // По умолчанию — "неизвестная пара"
        }

        // Заполняем существующие пары
        map[FlatIndex(Ticker::kBTC, Ticker::kUSDT)] = TradingPair::kBTCUSDT;
        map[FlatIndex(Ticker::kETH, Ticker::kUSDT)] = TradingPair::kETHUSDT;

        return map;
    }();

// Возвращает найденную пару, если существует
constexpr std::pair<bool, TradingPair> TickersToTradingPair(Ticker base,
                                                            Ticker quote) {
    TradingPair pair = kReverseMapFlat[FlatIndex(base, quote)];
    return {pair != TradingPair::kCount, pair};
}
};  // namespace aos