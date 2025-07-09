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

void wait_for_user_input(std::promise<void>& exit_signal) {
    std::cout << "Press ENTER to exit...\n";
    std::cin.get();           // ждём нажатия Enter
    exit_signal.set_value();  // снимаем блокировку
}

int main(int, char**) {
    {
        boost::asio::thread_pool thread_pool;
        try {
            using HashT = uint64_t;
            using Price = double;
            fmtlog::setLogLevel(fmtlog::LogLevel::DBG);
            // --- механизм блокировки через future ---
            std::promise<void> exit_promise;
            std::future<void> exit_future = exit_promise.get_future();

            // Поток, ожидающий пользовательский ввод
            std::jthread input_thread(wait_for_user_input,
                                      std::ref(exit_promise));
            // boost::asio::thread_pool thread_pool;
            LogPolling log_polling(thread_pool, std::chrono::microseconds(1));
            using Price = double;
            using Qty   = double;
            constexpr aos::NotifierFlags kNotifierOptionsBybitLinear{
                /*best_bid_enabled=*/false,
                /*best_ask_enabled=*/false,
                /*best_bid_price_enabled=*/true,
                /*best_ask_price_enabled=*/true,
            };
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
                aos::TradingPair::kETHBTC);
            binance_futures_main_net_infrastructure->Register(
                aos::TradingPair::kBTCUSDT);
            binance_futures_main_net_infrastructure->Register(
                aos::TradingPair::kETHUSDT);

            binance_futures_main_net_infrastructure->StartAsync();
            exit_future.get();  // блокирующий вызов — будет ждать ENTER
            binance_futures_main_net_infrastructure->StopAsync();
        } catch (...) {
            loge("error occured");
        }
    }
    logi("FINISHHHHHHHHHHHHHHHHHHHHHHHHHHH");
    fmtlog::poll();
    return 0;
}