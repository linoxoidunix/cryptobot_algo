#pragma once
#include <string_view>
#include <unordered_map>

#include "aos/trading_pair/trading_pair.h"

namespace aos {
namespace impl {
// Хэш-таблица для конвертации торговой пары в строку
class TradingPairToBigStringView {
    constexpr static std::array<std::pair<TradingPair, std::string_view>,
                                static_cast<std::size_t>(TradingPair::kCount)>
        kDictionary = {{
            {TradingPair::kBTCUSDT, "BTCUSDT"},
            {TradingPair::kETHUSDT, "ETHUSDT"},
            {TradingPair::kETHBTC, "ETHBTC"},
            {TradingPair::kBNBUSDT, "BNBUSDT"},
            {TradingPair::kBNBETH, "BNBETH"},
            {TradingPair::kSOLUSDT, "SOLUSDT"},
            {TradingPair::kSOLBNB, "SOLBNB"},
            {TradingPair::kXRPUSDT, "XRPUSDT"},
            {TradingPair::kADAUSDT, "ADAUSDT"},
            {TradingPair::kADAXRP, "ADAXRP"},
        }};

  public:
    virtual ~TradingPairToBigStringView() = default;
    // Используем constexpr хэш-таблицу для O(1) доступа
    static constexpr std::pair<bool, std::string_view> Convert(
        const TradingPair& trading_pair) {
        auto index = std::hash<TradingPair>{}(trading_pair);
        // Сравниваем хэш с предустановленными значениями
        if (index < kDictionary.size() &&
            kDictionary[index].first == trading_pair) {
            return {true, kDictionary[index].second};
        }
        return {false, std::string_view{}};
    }
};
};  // namespace impl
};  // namespace aos