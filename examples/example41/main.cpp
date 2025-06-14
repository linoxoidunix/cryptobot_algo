#include <thread>

#include "aoe/aoe.h"
#include "aoe/binance/hash_utils/hash_utils.h"
#include "aoe/binance/infrastructure/infrastructure.h"
#include "aoe/binance/order_book_sync/order_book_sync.h"
#include "aoe/binance/parser/json/ws/diff_response/parser.h"
#include "aoe/bybit/hash_utils/hash_utils.h"
#include "aos/aos.h"
#include "aos/common/exchange_id.h"
#include "aos/common/mem_pool.h"
#include "aos/logger/logger.h"
#include "aos/mutual_information/c_mutual_information_calculator.h"
#include "aos/sliding_window_storage/c_sliding_window_storage.h"
#include "aos/strategies/deviation_and_mutual_info/c_strategy.h"
#include "aos/trading_pair/trading_pair.h"
#include "fmtlog.h"

namespace str {
struct Config {
  public:
    aos::TradingPair trading_pair = aos::TradingPair::kBTCUSDT;
};
template <typename Price, typename Qty, typename HashT>
class StrategyWrapper {
    Config config_;
    aoe::binance::futures::main_net::InfrastructureNotifierInterface<
        Price, Qty>& infra_;
    aos::StrategyEngineInterface<HashT, Price>& strategy_engine_;
    bool strategy_init_success_ = false;

  public:
    StrategyWrapper(Config config,
             aoe::binance::futures::main_net::InfrastructureNotifierInterface<
                 Price, Qty>& infra, aos::StrategyEngineInterface<HashT, Price>& strategy_engine)
        : config_(config), infra_(infra), strategy_engine_(strategy_engine) {
        SetStrategy();
    }
    bool Run() {
        logi("Strategy run");
        return strategy_init_success_;
    }

  private:
    void SetStrategy() {
        bool status_set_cb_on_best_bid_change =
            infra_.SetCallbackOnBestBidChange(
                config_.trading_pair,
                [this](const aos::BestBid<Price, Qty>& new_best_bid) {
                    logi(
                        "[MY_ULTIMATE_FUTURES_STRATEGY] invoke callback "
                        "on best bid "
                        "change");
                    const auto binance_futures_main_net_btc_usdt = aoe::binance::HashKey(aoe::binance::Category::kFutures, aos::NetworkEnvironment::kMainNet, config_.trading_pair);  
                    strategy_engine_.AddData(binance_futures_main_net_btc_usdt, new_best_bid.bid_price);

                });
        bool status_set_cb_on_best_ask_change =
            infra_.SetCallbackOnBestAskChange(
                config_.trading_pair,
                [this](const aos::BestAsk<Price, Qty>& new_best_ask) {
                    logi(
                        "[MY_ULTIMATE_FUTURES_STRATEGY] invoke callback on "
                        "best ask "
                        "change");
                    const auto binance_futures_main_net_btc_usdt = aoe::binance::HashKey(aoe::binance::Category::kFutures, aos::NetworkEnvironment::kMainNet, config_.trading_pair);  
                    strategy_engine_.AddData(binance_futures_main_net_btc_usdt, new_best_ask.ask_price);
                });
        strategy_init_success_ =
            status_set_cb_on_best_bid_change & status_set_cb_on_best_ask_change;
    }
};
};  // namespace str

int main(int argc, char** argv) {
    {
        using HashT = uint64_t;
        using Price = double;
        fmtlog::setLogLevel(fmtlog::LogLevel::DBG);
        boost::asio::thread_pool thread_pool;
        LogPolling log_polling(thread_pool, std::chrono::microseconds(1));
        using Price = double;
        using Qty   = double;
        aoe::binance::impl::main_net::futures::Infrastructure<
            Price, Qty, common::MemoryPoolThreadSafety,
            common::MemoryPoolNotThreadSafety>
            infrastructure(thread_pool);
        infrastructure.Register(aos::TradingPair::kBTCUSDT);
        
        aoe::bybit::impl::main_net::linear::Infrastructure<
            Price, Qty, common::MemoryPoolThreadSafety,
            common::MemoryPoolNotThreadSafety>
            infrastructure(thread_pool);
        infrastructure.Register(aos::TradingPair::kBTCUSDT);
        //init strategy
        
        aos::impl::MarketTripletManagerDefault<HashT> market_triplet_manager;
        const auto binance_btc_usdt = aoe::binance::HashKey(
            aoe::binance::Category::kFutures, aos::NetworkEnvironment::kMainNet,
            aos::TradingPair::kBTCUSDT);
        const auto bybit_btc_usdt = aoe::bybit::HashKey(
            aoe::bybit::Category::kLinear, aos::NetworkEnvironment::kMainNet,
            aos::TradingPair::kBTCUSDT);
        market_triplet_manager.Connect(binance_btc_usdt, bybit_btc_usdt);

        constexpr aos::strategies::deviation_and_mutual_information::Config<
            HashT>
            config_strategy{5, 10, binance_btc_usdt};
        aos::strategies::deviation_and_mutual_information::Strategy<HashT,
                                                                    Price>
            strategy(market_triplet_manager, config_strategy);

        aos::impl::StrategyEngineDefault<HashT, Price> strategy_engine(thread_pool,
                                                            strategy);
        
        str::Config config;
        str::StrategyWrapper<Price, Qty, HashT> strategy_wrapper(config, infrastructure, strategy_engine);
        auto status = strategy_wrapper.Run();
        if (!status) {
            logi("Strategy not init successful");
            return 0;
        }
    }
    logi("FINISHHHHHHHHHHHHHHHHHHHHHHHHHHH");
    fmtlog::poll();
    return 0;
}