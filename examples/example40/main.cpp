#include <boost/asio/thread_pool.hpp>

#include "aoe/binance/hash_utils/hash_utils.h"
#include "aoe/bybit/hash_utils/hash_utils.h"
#include "aos/logger/mylog.h"
#include "aos/market_triplet_manager/market_triplet_manager.h"
#include "aos/mutual_information/c_mutual_information_calculator.h"
#include "aos/sliding_window_storage/c_sliding_window_storage.h"
#include "aos/strategies/deviation_and_mutual_info/c_strategy.h"
#include "aos/strategy_engine/strategy_engine.h"
// qweqwe

int main() {
    {
        try {
            using HashT = uint64_t;
            using Price = double;
            using Qty   = double;
            aos::impl::MarketTripletManagerDefault<HashT>
                market_triplet_manager;
            const auto binance_btc_usdt = aoe::binance::HashKey(
                aoe::binance::Category::kSpot,
                aos::NetworkEnvironment::kMainNet, aos::TradingPair::kBTCUSDT);
            const auto bybit_btc_usdt = aoe::bybit::HashKey(
                aoe::bybit::Category::kSpot, aos::NetworkEnvironment::kMainNet,
                aos::TradingPair::kBTCUSDT);
            market_triplet_manager.Connect(binance_btc_usdt, bybit_btc_usdt);

            boost::asio::thread_pool thread_pool;

            constexpr aos::strategies::deviation_and_mutual_information::Config<
                HashT>
                kConfig{5, 10, binance_btc_usdt, 0.001, 1};
            aos::strategies::deviation_and_mutual_information::Strategy<HashT,
                                                                        Price>
                strategy(market_triplet_manager, kConfig);
            strategy.Init();

            aos::impl::StrategyEngineDefault<HashT, Price> strategy_engine(
                thread_pool, strategy);
            std::vector<std::pair<double, double>> incoming_data = {
                {29300.25, 29290.10}, {29350.50, 29340.35},
                {29400.75, 29380.20}, {29450.00, 29430.15},
                {29500.25, 29488.05}, {29550.50, 29520.40},
                {29602.75, 29560.60}, {29000.00, 29600.50},
                {49700.25, 29650.40}, {29750.50, 59700.35}};
            for (const auto& it : incoming_data) {
                strategy_engine.AddData(binance_btc_usdt, it.first);
                strategy_engine.AddData(bybit_btc_usdt, it.second);
            }
            strategy_engine.Wait();
        } catch (...) {
            loge("error occured");
        }
    }
    fmtlog::poll();
    return 0;
}