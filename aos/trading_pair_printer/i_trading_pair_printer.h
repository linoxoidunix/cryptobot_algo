#pragma once
#include "aos/trading_pair/trading_pair.h"
#include "aot/common/types.h"
namespace aos {
class TradingPairPrinterInterface {
  public:
    virtual std::pair<bool, std::string_view> Print(
        const aos::TradingPair& trading_pair) = 0;
    virtual ~TradingPairPrinterInterface()    = default;
};
};  // namespace aos