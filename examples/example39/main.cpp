#include "aoe/binance/hash_utils/hash_utils.h"
#include "aoe/bybit/hash_utils/hash_utils.h"
#include "aos/hash_utils/decompose_hash.h"
#include "fmtlog.h"

int main() {
    {
        auto hash1 = aoe::binance::HashKey(aoe::binance::Category::kFutures,
                                           aos::NetworkEnvironment::kMainNet,
                                           aos::TradingPair::kETHUSDT);
        auto hash2 = aoe::bybit::HashKey(aoe::bybit::Category::kSpot,
                                         aos::NetworkEnvironment::kTestNet,
                                         aos::TradingPair::kBTCUSDT);
        common::ExchangeId exchange_id_1 = common::ExchangeId::kInvalid;
        aos::NetworkEnvironment network_environment_1;
        aos::CategoryRaw category_market_1;
        aos::TradingPair trading_pair_1;
        aos::DecomposeHash(hash1, exchange_id_1, category_market_1,
                           network_environment_1, trading_pair_1);
        logi("hash1 = {} {} category={} {} {}",
             hash1, exchange_id_1, category_market_1, network_environment_1,
             trading_pair_1);
        common::ExchangeId exchange_id_2 = common::ExchangeId::kInvalid;
        aos::NetworkEnvironment network_environment_2;
        aos::CategoryRaw category_market_2;
        aos::TradingPair trading_pair_2;
        aos::DecomposeHash(hash2, exchange_id_2, category_market_2,
                           network_environment_2, trading_pair_2);
        logi("hash2 = {} {} category={} {} {}",
             hash2, exchange_id_2, category_market_2, network_environment_2,
             trading_pair_2);
    }
    fmtlog::poll();
    return 0;
}