#pragma once
#include <unordered_map>

#include "aos/trading_pair_printer/i_trading_pair_printer.h"
#include "aot/common/types.h"

namespace aos {
namespace impl {
namespace ws {
namespace order_book_channel {
class TradingPairPrinter : public TradingPairPrinterInterface {
    std::unordered_map<common::TradingPair, std::string,
                       common::TradingPairHash, common::TradingPairEqual>
        dictionary_;

  public:
    TradingPairPrinter() { dictionary_[{2, 1}] = "BTCUSDT"; }
    std::pair<bool, std::string_view> Print(
        const common::TradingPair& trading_pair) override {
        auto it = dictionary_.find(trading_pair);
        if (it == dictionary_.end())
            return std::make_pair(false, std::string_view{});
        return std::make_pair(true, std::string_view(it->second));
    };
};
};  // namespace order_book_channel
};  // namespace ws
};  // namespace impl
};  // namespace aos