#include <boost/asio.hpp>

#include "aoe/api/exchange_api.h"
#include "aoe/binance/hash_utils/hash_utils.h"
#include "aos/common/mem_pool.h"
#include "aos/market_triplet_manager/market_triplet_manager.h"
#include "aos/mutual_information/c_mutual_information_calculator.h"
#include "aos/sliding_window_storage/c_sliding_window_storage.h"
#include "aos/strategies/deviation_and_mutual_info/c_strategy.h"
#include "aos/strategy_engine/strategy_engine.h"
#include "fmt/format.h"  //NOLINT
#include "fmt/ranges.h"  // NOLINT
#include "fmtlog.h"      // NOLINT

int main() {
    {
        try {
            using HashT = std::size_t;
            using Price = double;
            using Qty   = double;
            aos::impl::MarketTripletManagerDefault<HashT>
                market_triplet_manager;
            market_triplet_manager.Connect(1, 2);

            boost::asio::thread_pool thread_pool;
            const auto binance_btc_usdt = aoe::binance::HashKey(
                aoe::binance::Category::kFutures,
                aos::NetworkEnvironment::kMainNet, aos::TradingPair::kBTCUSDT);

            aoe::impl::PlaceOrderDummy<common::MemoryPoolThreadSafety>
                place_order_dummy;

            constexpr aos::strategies::deviation_and_mutual_information::Config<
                HashT>
                kConfigStrategy{5, 10, binance_btc_usdt, 0.001, 2};

            aos::strategies::deviation_and_mutual_information::Strategy<
                HashT, Price, std::unordered_map<HashT, Price>,
                common::MemoryPoolThreadSafety>
                strategy(market_triplet_manager, kConfigStrategy,
                         place_order_dummy);
            strategy.Init();
            aos::impl::StrategyEngineDefault<HashT, Price> strategy_engine(
                thread_pool, strategy);
            std::vector<std::pair<double, double>> incoming_data = {
                {29300.25, 29290.10}, {29350.50, 29340.35},
                {29400.75, 29380.20}, {29450.00, 29430.15},
                {29500.25, 29488.05}, {29550.50, 29520.40},
                {29602.75, 29560.60}, {29000.00, 29600.50},
                {49700.25, 29650.40}, {29750.50, 59700.35}};
            size_t hash_first_asset  = 1;
            size_t hash_second_asset = 2;
            for (const auto& it : incoming_data) {
                strategy_engine.AddData(hash_first_asset, it.first);
                strategy_engine.AddData(hash_second_asset, it.second);
            }
            strategy_engine.Wait();
        } catch (...) {
            loge("error occured");
        }
    }
    fmtlog::poll();
    return 0;
}