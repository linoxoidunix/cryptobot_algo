#include <optional>
#include <thread>
#include <unordered_map>

#include "aoe/binance/hash_utils/hash_utils.h"
#include "aoe/binance/infrastructure/infrastructure.h"
#include "aoe/bybit/hash_utils/hash_utils.h"
#include "aoe/bybit/infrastructure/infrastructure.h"
#include "aos/common/exchange_id.h"
#include "aos/common/mem_pool.h"
#include "aos/logger/logger.h"
#include "aos/logger/mylog.h"
#include "aos/market_triplet_manager/market_triplet_manager.h"
#include "aos/order_book_notifier_flags/flags.h"
#include "aos/strategies/deviation_and_mutual_info/c_strategy.h"
#include "aos/strategy_engine/i_strategy_engine.h"
#include "aos/strategy_engine/strategy_engine.h"
#include "aos/trading_pair/trading_pair.h"

namespace str {
struct Config {
  public:
    aos::TradingPair trading_pair = aos::TradingPair::kBTCUSDT;
};
template <typename Price, typename Qty, typename HashT>
class StrategyWrapper {
    Config config_;
    aoe::binance::futures::main_net::InfrastructureNotifierInterface<
        Price, Qty>& binance_futures_mainnet_infra_;
    aoe::bybit::linear::main_net::InfrastructureNotifierInterface<Price, Qty>&
        bybit_linear_mainnet_infra_;
    aos::StrategyEngineInterface<HashT, Price>& strategy_engine_;
    bool strategy_init_success_ = false;

  public:
    StrategyWrapper(
        Config config,
        aoe::binance::futures::main_net::InfrastructureNotifierInterface<
            Price, Qty>& binance_futures_mainnet_infra,
        aoe::bybit::linear::main_net::InfrastructureNotifierInterface<
            Price, Qty>& bybit_linear_mainnet_infra,
        aos::StrategyEngineInterface<HashT, Price>& strategy_engine)
        : config_(config),
          binance_futures_mainnet_infra_(binance_futures_mainnet_infra),
          bybit_linear_mainnet_infra_(bybit_linear_mainnet_infra),
          strategy_engine_(strategy_engine) {}
    bool Init() {
        logi("Strategy init");
        SetStrategy();
        return strategy_init_success_;
    }
    ~StrategyWrapper() { logi("d'tor strategy wrapper"); }

  private:
    void SetStrategy() {
        bool
            status_set_cb_on_best_bid_change_with_binance_futures_mainnet_infra =
                binance_futures_mainnet_infra_.SetCallbackOnBestBidPriceChange(
                    config_.trading_pair, [this](Price& new_best_bid_price) {
                        logi(
                            "[MY_ULTIMATE_FUTURES_STRATEGY] invoke callback "
                            "on best bid "
                            "change");
                        const auto binance_futures_main_net_btc_usdt =
                            aoe::binance::HashKey(
                                aoe::binance::Category::kFutures,
                                aos::NetworkEnvironment::kMainNet,
                                config_.trading_pair);
                        strategy_engine_.AddData(
                            binance_futures_main_net_btc_usdt,
                            new_best_bid_price);
                    });
        bool
            status_set_cb_on_best_ask_change_with_binance_futures_mainnet_infra =
                binance_futures_mainnet_infra_.SetCallbackOnBestAskPriceChange(
                    config_.trading_pair, [this](Price& new_best_ask_price) {
                        logi(
                            "[MY_ULTIMATE_FUTURES_STRATEGY] invoke callback on "
                            "best ask "
                            "change");
                        const auto binance_futures_main_net_btc_usdt =
                            aoe::binance::HashKey(
                                aoe::binance::Category::kFutures,
                                aos::NetworkEnvironment::kMainNet,
                                config_.trading_pair);
                        strategy_engine_.AddData(
                            binance_futures_main_net_btc_usdt,
                            new_best_ask_price);
                    });
        bool status_set_cb_on_best_bid_change_with_bybit_linear_mainnet_infra =
            bybit_linear_mainnet_infra_.SetCallbackOnBestBidPriceChange(
                config_.trading_pair, [this](Price& new_best_bid_price) {
                    logi(
                        "[MY_ULTIMATE_FUTURES_STRATEGY] invoke callback "
                        "on best bid "
                        "change");
                    const auto bybit_linear_main_net_btc_usdt =
                        aoe::bybit::HashKey(aoe::bybit::Category::kLinear,
                                            aos::NetworkEnvironment::kMainNet,
                                            config_.trading_pair);
                    strategy_engine_.AddData(bybit_linear_main_net_btc_usdt,
                                             new_best_bid_price);
                    strategy_engine_.SetBidPrice(bybit_linear_main_net_btc_usdt,
                                                 new_best_bid_price);
                });
        bool status_set_cb_on_best_ask_change_with_bybit_linear_mainnet_infra =
            bybit_linear_mainnet_infra_.SetCallbackOnBestAskPriceChange(
                config_.trading_pair, [this](Price& new_best_ask_price) {
                    logi(
                        "[MY_ULTIMATE_FUTURES_STRATEGY] invoke callback "
                        "on best ask "
                        "change");
                    const auto bybit_linear_main_net_btc_usdt =
                        aoe::bybit::HashKey(aoe::bybit::Category::kLinear,
                                            aos::NetworkEnvironment::kMainNet,
                                            config_.trading_pair);
                    strategy_engine_.AddData(bybit_linear_main_net_btc_usdt,
                                             new_best_ask_price);
                    strategy_engine_.SetAskPrice(bybit_linear_main_net_btc_usdt,
                                                 new_best_ask_price);
                });
        strategy_init_success_ =
            status_set_cb_on_best_bid_change_with_binance_futures_mainnet_infra &&
            status_set_cb_on_best_ask_change_with_binance_futures_mainnet_infra &&
            status_set_cb_on_best_bid_change_with_bybit_linear_mainnet_infra &&
            status_set_cb_on_best_ask_change_with_bybit_linear_mainnet_infra;
    }
};
};  // namespace str

int main(int, char**) {
    {
        try {
            using HashT = uint64_t;
            using Price = double;
            fmtlog::setLogLevel(fmtlog::LogLevel::DBG);
            boost::asio::thread_pool thread_pool;
            LogPolling log_polling(thread_pool, std::chrono::microseconds(1));
            using Price = double;
            using Qty   = double;
            constexpr aos::NotifierFlags kNotifierOptionsBybitLinear{
                /*best_bid_enabled=*/false,
                /*best_ask_enabled=*/false,
                /*best_bid_price_enabled=*/true,
                /*best_ask_price_enabled=*/true,
            };
            std::optional<aoe::bybit::impl::main_net::linear::Infrastructure<
                Price, Qty, common::MemoryPoolThreadSafety,
                common::MemoryPoolNotThreadSafety, kNotifierOptionsBybitLinear>>
                bybit_linear_mainnet_infrastructure;
            // strategy
            aos::impl::MarketTripletManagerDefault<HashT>
                market_triplet_manager;
            const auto binance_btc_usdt = aoe::binance::HashKey(
                aoe::binance::Category::kFutures,
                aos::NetworkEnvironment::kMainNet, aos::TradingPair::kBTCUSDT);
            const auto bybit_btc_usdt = aoe::bybit::HashKey(
                aoe::bybit::Category::kLinear,
                aos::NetworkEnvironment::kMainNet, aos::TradingPair::kBTCUSDT);
            market_triplet_manager.Connect(binance_btc_usdt, bybit_btc_usdt);

            constexpr aos::strategies::deviation_and_mutual_information::Config<
                HashT>
                kConfigStrategy{5, 10, binance_btc_usdt, 0.00000001, 1};
            aos::strategies::deviation_and_mutual_information::Strategy<
                HashT, Price, std::unordered_map<HashT, Price>,
                common::MemoryPoolThreadSafety>
                strategy(market_triplet_manager, kConfigStrategy,
                         *bybit_linear_mainnet_infrastructure);

            constexpr aos::NotifierFlags kNotifierOptionsBinanceFutures{
                /*best_bid_enabled=*/false,
                /*best_ask_enabled=*/false,
                /*best_bid_price_enabled=*/true,
                /*best_ask_price_enabled=*/true,
            };
            std::optional<aoe::binance::impl::main_net::futures::Infrastructure<
                Price, Qty, common::MemoryPoolThreadSafety,
                common::MemoryPoolNotThreadSafety,
                kNotifierOptionsBinanceFutures>>
                binance_futures_main_net_infrastructure;
            binance_futures_main_net_infrastructure.emplace(thread_pool);
            binance_futures_main_net_infrastructure->Register(
                aos::TradingPair::kBTCUSDT);

            bybit_linear_mainnet_infrastructure.emplace(thread_pool);
            bybit_linear_mainnet_infrastructure->Register(
                aos::TradingPair::kBTCUSDT);

            strategy.Init();
            aos::impl::StrategyEngineDefault<HashT, Price> strategy_engine(
                thread_pool, strategy);
            // init strategy

            str::Config config;
            str::StrategyWrapper<Price, Qty, HashT> strategy_wrapper(
                config, *binance_futures_main_net_infrastructure,
                *bybit_linear_mainnet_infrastructure, strategy_engine);
            auto status = strategy_wrapper.Init();
            if (!status) {
                logi("Strategy not init successful");
                return 0;
            }
            bybit_linear_mainnet_infrastructure->StartAsync();
            binance_futures_main_net_infrastructure->StartAsync();
            bybit_linear_mainnet_infrastructure.reset();
            binance_futures_main_net_infrastructure.reset();
        } catch (...) {
            loge("error occured");
        }
    }
    logi("FINISHHHHHHHHHHHHHHHHHHHHHHHHHHH");
    fmtlog::poll();
    return 0;
}