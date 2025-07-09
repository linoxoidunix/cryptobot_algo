#pragma once
#include <string_view>
#include <unordered_map>

#include "aos/trading_pair/trading_pair.h"

namespace aos {
namespace impl {
// Хэш-таблица для конвертации торговой пары в строку
class BigStringViewToTradingPair {
    static const inline std::unordered_map<std::string_view, TradingPair>
        kDictionary = {{"BTCUSDT", aos::TradingPair::kBTCUSDT},
                       {"ETHUSDT", aos::TradingPair::kETHUSDT},
                       {"ETHBTC", aos::TradingPair::kETHBTC},
                       {"BNBUSDT", aos::TradingPair::kBNBUSDT},
                       {"BNBETH", aos::TradingPair::kBNBETH},
                       {"SOLUSDT", aos::TradingPair::kSOLUSDT},
                       {"SOLBNB", aos::TradingPair::kSOLBNB},
                       {"XRPUSDT", aos::TradingPair::kXRPUSDT},
                       {"ADAUSDT", aos::TradingPair::kADAUSDT},
                       {"ADAXRP", aos::TradingPair::kADAXRP}};

  public:
    virtual ~BigStringViewToTradingPair() = default;
    // Используем constexpr хэш-таблицу для O(1) доступа
    static std::pair<bool, TradingPair> Convert(std::string_view trading_pair) {
        if (auto it = kDictionary.find(trading_pair); it != kDictionary.end()) {
            logd("found {} for {}", it->second, trading_pair);
            return {true, it->second};
        }
        logd("not found {} size_dict:{}", trading_pair, kDictionary.size());
        return {false, TradingPair::kCount};
    }
};
};  // namespace impl
};  // namespace aos