#pragma once
#include <unordered_map>

#include "aos/trading_pair_printer/i_trading_pair_printer.h"
#include "aot/common/types.h"
namespace aos {
namespace impl {
class TradingPairPrinter : public TradingPairPrinterInterface {
    std::unordered_map<common::TradingPair, std::string,
                       common::TradingPairHash, common::TradingPairEqual>
        dictionary_;

  public:
    TradingPairPrinter() { dictionary_[{2, 1}] = "BTCUSDT"; }
    std::pair<bool, std::string_view> Print(
        const common::TradingPair& trading_pair) override {
        if (!dictionary_.contains(trading_pair))
            return std::make_pair(false, std::string_view{});
        return std::make_pair(true, dictionary_.at(trading_pair));
    };
};
};  // namespace impl
};  // namespace aos