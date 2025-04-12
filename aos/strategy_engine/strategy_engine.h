#pragma once

#include "aos/strategy/i_strategy.h"

namespace aos {
namespace impl {
template <typename HashT, typename T>
class StrategyEngineDefault : public StrategyEngineInterface {
    boost::asio::strand<boost::asio::thread_pool::executor_type>
        strand_;  // Strand для последовательного выполнения
    boost::asio::thread_pool& thread_pool_;
    StrategyInterface& strategy_;
    SlidingWindowStorageInterface& sliding_window_storage_;
    boost::asio::strand<boost::asio::thread_pool::executor_type>
        strand_;  // Strand для последовательного выполнения
  public:
    StrategyEngineDefault(boost::asio::thread_pool& thread_pool,
                          StrategyInterface& strategy,
                          SlidingWindowStorageInterface& sliding_window_storage)
        : strategy_(strategy),
          sliding_window_storage_(sliding_window_storage),
          strand_(boost::asio::make_strand(thread_pool.get_executor())) {}
    void AddData(const HashT asset, const T& value) override {
        logi("Add hash:{} value:{} to strand", asset, value);
        boost::asio::post(strand_, [this, asset, value]() {
            strategy_.AnalyzeToBuy(asset, value);
            strategy_.AnalyzeToSell(asset, value);
            sliding_window_storage_.AddData(asset, value);
        });
    }
};
};  // namespace impl
};  // namespace aos