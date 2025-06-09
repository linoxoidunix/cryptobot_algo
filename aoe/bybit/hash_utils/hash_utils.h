#pragma once
#include <cstdint>

#include "aoe/bybit/enums/enums.h"
#include "aos/common/exchange_id.h"
#include "aos/common/network_environment.h"
#include "aos/trading_pair/trading_pair.h"

namespace aoe {
namespace bybit {
constexpr uint64_t HashKey(Category market,
                           aos::NetworkEnvironment net, aos::TradingPair pair) {
    return (static_cast<uint64_t>(common::ExchangeId::kBybit) << 48) |
           (static_cast<uint64_t>(market) << 40) |
           (static_cast<uint64_t>(net) << 32) | static_cast<uint64_t>(pair);
};
};  // namespace bybit
};  // namespace aoe
