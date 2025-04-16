#pragma once
#include <string_view>

#include "aot/common/types.h"

namespace aos {
class TradingPairFactoryInterface {
  public:
    virtual std::pair<bool, common::TradingPair> Produce(std::string_view trading_pair) = 0;
    virtual ~TradingPairFactoryInterface() = default;
};
};  // namespace aos