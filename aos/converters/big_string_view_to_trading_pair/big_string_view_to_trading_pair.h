#pragma once
#include <string_view>
#include <unordered_map>

#include "aos/trading_pair/trading_pair.h"

namespace aos {
namespace impl {
// Хэш-таблица для конвертации торговой пары в строку
class BigStringViewToTradingPair {
    constexpr static std::array<std::pair<std::string_view, TradingPair>, 3>
        dictionary_ = {{
            {"BTCUSDT", TradingPair::kBTCUSDT},
            {"ETHUSDT", TradingPair::kETHUSDT},
            {"SOLUSDT", TradingPair::kSOLUSDT},
        }};

  public:
    virtual ~BigStringViewToTradingPair() = default;
    // Используем constexpr хэш-таблицу для O(1) доступа
    constexpr std::pair<bool, TradingPair> Convert(
        std::string_view trading_pair) const {
        auto index = std::hash<std::string_view>{}(trading_pair);
        // Сравниваем хэш с предустановленными значениями
        if (index < dictionary_.size() &&
            dictionary_[index].first == trading_pair) {
            return {true, dictionary_[index].second};
        }
        return {false, std::string_view{}};
    }
};
};  // namespace impl
};  // namespace aos