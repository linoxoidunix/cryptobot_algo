#pragma once
#include <string_view>
#include <unordered_map>

#include "aos/trading_pair/trading_pair.h"

namespace aos {
namespace impl {
// Хэш-таблица для конвертации торговой пары в строку
class TradingPairToSmallStringView {
    constexpr static std::array<
        std::pair<aos::TradingPair, std::string_view>,
        static_cast<std::underlying_type_t<aos::TradingPair>>(
            aos::TradingPair::kCount)>
        kDictionary = {{
            {TradingPair::kBTCUSDT, "btcusdt"},
            {TradingPair::kETHUSDT, "ethusdt"},
            {TradingPair::kETHBTC, "ethbtc"},
            {TradingPair::kBNBUSDT, "bnbusdt"},
            {TradingPair::kBNBETH, "bnbeth"},
            {TradingPair::kSOLUSDT, "solusdt"},
            {TradingPair::kSOLBNB, "solbnb"},
            {TradingPair::kXRPUSDT, "xrpusdt"},
            {TradingPair::kADAUSDT, "adausdt"},
            {TradingPair::kADAXRP, "adaxrp"},
        }};

  public:
    virtual ~TradingPairToSmallStringView() = default;
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