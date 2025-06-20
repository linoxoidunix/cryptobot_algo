#include <thread>

#include "aoe/binance/infrastructure/infrastructure.h"
#include "aoe/binance/order_book_sync/order_book_sync.h"
#include "aoe/binance/parser/json/ws/diff_response/parser.h"
#include "aos/common/exchange_id.h"
#include "aos/common/mem_pool.h"
#include "aos/logger/logger.h"
#include "aos/logger/mylog.h"
#include "aos/trading_pair/trading_pair.h"

namespace str {
struct Config {
  public:
    aos::TradingPair trading_pair = aos::TradingPair::kBTCUSDT;
};
template <typename Price, typename Qty>
class Strategy {
    Config config_;
    aoe::binance::spot::main_net::InfrastructureNotifierInterface<Price, Qty>&
        infra_;
    bool strategy_init_success_ = false;

  public:
    Strategy(Config config,
             aoe::binance::spot::main_net::InfrastructureNotifierInterface<
                 Price, Qty>& infra)
        : config_(config), infra_(infra) {
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
                config_.trading_pair, [](const aos::BestBid<Price, Qty>&) {
                    logi(
                        "[MY_SUPER_PUPER_STRATEGY] invoke callback on best bid "
                        "change");
                });
        bool status_set_cb_on_best_change = infra_.SetCallbackOnBestAskChange(
            config_.trading_pair, [](const aos::BestAsk<Price, Qty>&) {
                logi(
                    "[MY_SUPER_PUPER_STRATEGY] invoke callback on best ask "
                    "change");
            });
        strategy_init_success_ =
            status_set_cb_on_best_bid_change && status_set_cb_on_best_change;
    }
};
};  // namespace str

int main(int, char**) {
    {
        try {
            fmtlog::setLogLevel(fmtlog::LogLevel::DBG);
            logd("{}", aos::TradingPair::kBTCUSDT);

            boost::asio::thread_pool thread_pool;
            LogPolling log_polling(thread_pool, std::chrono::microseconds(1));
            using Price = double;
            using Qty   = double;
            aoe::binance::impl::main_net::spot::Infrastructure<
                Price, Qty, common::MemoryPoolThreadSafety,
                common::MemoryPoolNotThreadSafety>
                infrastructure(thread_pool);
            infrastructure.Register(aos::TradingPair::kBTCUSDT);
            str::Config config;
            str::Strategy<Price, Qty> strategy(config, infrastructure);
            auto status = strategy.Run();
            if (!status) {
                logi("Strategy not init successful");
                return 0;
            }
        } catch (...) {
            loge("error occured");
        }
    }
    logi("FINISHHHHHHHHHHHHHHHHHHHHHHHHHHH");
    fmtlog::poll();
    return 0;
}