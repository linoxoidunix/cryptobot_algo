#pragma once
#include <string_view>
#include <unordered_map>

#include "aos/converters/trading_pair_to_big_string_view/i_trading_pair_to_big_string_view.h"

namespace aos {
namespace impl {
// Хэш-таблица для конвертации торговой пары в строку
class TradingPairToBigStringView {
    constexpr static std::array<std::pair<TradingPair, std::string_view>, 2>
        dictionary_ = {{
            {TradingPair::kBTCUSDT, "BTCUSDT"},
            {TradingPair::kETHUSDT, "ETHUSDT"},
        }};

  public:
    virtual ~TradingPairToBigStringView() = default;
    // Используем constexpr хэш-таблицу для O(1) доступа
    constexpr std::pair<bool, std::string_view> Convert(
        const TradingPair& trading_pair) const {
        auto index = std::hash<TradingPair>{}(trading_pair);
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