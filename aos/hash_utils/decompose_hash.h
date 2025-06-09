#pragma once
#include <cstdint>

#include "aos/common/exchange_id.h"
#include "aos/common/network_environment.h"
#include "aos/trading_pair/trading_pair.h"
#include "aos/common/category_market.h"

namespace aos {
/**
 * @brief Decomposes a hash back into its original components
 * 
 * @param hash The 64-bit hash value to decompose
 * @param out_exchange_id [out] Will contain the exchange ID (should be kBybit for valid hashes)
 * @param out_category_market [out] Will contain the market category (spot, futures, etc.)
 * @param out_net [out] Will contain the network environment
 * @param out_trading_pair [out] Will contain the trading pair
 * 
 * @note The function performs bitmask operations that are inverse to HashKey()'s operations
 */
inline void DecomposeHash(uint64_t hash,
                         common::ExchangeId& out_exchange_id,
                         aos::CategoryRaw& out_category_market,
                         aos::NetworkEnvironment& out_net, 
                         aos::TradingPair& out_trading_pair) {
    out_trading_pair = static_cast<aos::TradingPair>(hash & 0xFFFFFFFF);
    out_net = static_cast<aos::NetworkEnvironment>((hash >> 32) & 0xFF);
    out_category_market = static_cast<aos::CategoryRaw>((hash >> 40) & 0xFF);
    out_exchange_id = static_cast<common::ExchangeId>((hash >> 48) & 0xFF);
};
};  // namespace aoe