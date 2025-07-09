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
        map[FlatIndex(Ticker::kETH, Ticker::kBTC)]  = TradingPair::kETHBTC;
        map[FlatIndex(Ticker::kETH, Ticker::kBNB)]  = TradingPair::kETHBNB;
        map[FlatIndex(Ticker::kBTC, Ticker::kETH)]  = TradingPair::kBTCETH;
        map[FlatIndex(Ticker::kBNB, Ticker::kUSDT)] = TradingPair::kBNBUSDT;
        map[FlatIndex(Ticker::kBNB, Ticker::kETH)]  = TradingPair::kBNBETH;
        map[FlatIndex(Ticker::kSOL, Ticker::kUSDT)] = TradingPair::kSOLUSDT;
        map[FlatIndex(Ticker::kSOL, Ticker::kBNB)]  = TradingPair::kSOLBNB;
        map[FlatIndex(Ticker::kXRP, Ticker::kUSDT)] = TradingPair::kXRPUSDT;
        map[FlatIndex(Ticker::kADA, Ticker::kUSDT)] = TradingPair::kADAUSDT;
        map[FlatIndex(Ticker::kADA, Ticker::kXRP)]  = TradingPair::kADAXRP;

        return map;
    }();

// Возвращает найденную пару, если существует
constexpr std::pair<bool, TradingPair> TickersToTradingPair(Ticker base,
                                                            Ticker quote) {
    TradingPair pair = kReverseMapFlat[FlatIndex(base, quote)];
    return {pair != TradingPair::kCount, pair};
}
};  // namespace aos
