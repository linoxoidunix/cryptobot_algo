#pragma once
#include "aos/trading_pair/trading_pair.h"
#include "aot/common/types.h"

namespace aos {
class TradingPairToBigStringViewInterface {
  public:
    virtual constexpr std::pair<bool, std::string_view> Convert(
        const TradingPair& trading_pair)           = 0;
    virtual ~TradingPairToBigStringViewInterface() = default;
};
};  // namespace aos