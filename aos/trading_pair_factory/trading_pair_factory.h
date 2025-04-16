#pragma once
#include <unordered_map>

#include "aos/trading_pair_factory/i_trading_pair_factory.h"

namespace aos {
namespace impl {
class TradingPairFactoryTest : public TradingPairFactoryInterface {
    std::unordered_map<std::string_view, common::TradingPair> dictionary_;

  public:
    TradingPairFactoryTest() { dictionary_["BTCUSDT"] = {2, 1}; };
    std::pair<bool, common::TradingPair> Produce(
        std::string_view trading_pair) {
        if (!dictionary_.contains(trading_pair))
            return std::make_pair(false, common::TradingPair{});
        return std::make_pair(true, dictionary_.at(trading_pair));
    }
    ~TradingPairFactoryTest() override {}
};
};  // namespace impl
};  // namespace aos