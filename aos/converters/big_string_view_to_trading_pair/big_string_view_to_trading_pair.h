#pragma once
#include <string_view>
#include <unordered_map>

#include "aos/trading_pair/trading_pair.h"

namespace aos {
namespace impl {
// Хэш-таблица для конвертации торговой пары в строку
class BigStringViewToTradingPair {
    static const inline std::unordered_map<std::string_view, TradingPair>
        dictionary_ = {
            {"BTCUSDT", TradingPair::kBTCUSDT},
            {"ETHUSDT", TradingPair::kETHUSDT},
            {"SOLUSDT", TradingPair::kSOLUSDT},
    };

  public:
    virtual ~BigStringViewToTradingPair() = default;
    // Используем constexpr хэш-таблицу для O(1) доступа
    std::pair<bool, TradingPair> Convert(std::string_view trading_pair) const {
        if (auto it = dictionary_.find(trading_pair); it != dictionary_.end()) {
            logd("found {} for {}", it->second, trading_pair);
            return {true, it->second};
        }
        logd("not found {} size_dict:{}", trading_pair, dictionary_.size());
        return {false, TradingPair::kCount};
    }
};
};  // namespace impl
};  // namespace aos