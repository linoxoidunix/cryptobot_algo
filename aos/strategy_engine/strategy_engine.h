#pragma once
#include "aos/sliding_window_storage/i_sliding_window_storage.h"
#include "aos/strategy/i_strategy.h"
#include "aos/strategy_engine/i_strategy_engine.h"
#include "boost/asio.hpp"

namespace aos {
namespace impl {
template <typename HashT, typename T>
class StrategyEngineDefault : public StrategyEngineInterface<HashT, T> {
    boost::asio::strand<boost::asio::thread_pool::executor_type>
        strand_;  // Strand для последовательного выполнения
    StrategyInterface<HashT, T>& strategy_;

  public:
    StrategyEngineDefault(boost::asio::thread_pool& thread_pool,
                          StrategyInterface<HashT, T>& strategy)
        : strand_(boost::asio::make_strand(thread_pool.get_executor())),
          strategy_(strategy) {}
    void AddData(const HashT& asset, const T& value) override {
        logi("Add hash:{} value:{} to strand", asset, value);
        boost::asio::post(strand_, [this, asset, value]() {
            strategy_.AnalyzeToBuy(asset, value);
            strategy_.AnalyzeToSell(asset, value);
            strategy_.AddData(asset, value);
        });
    }
    void Wait() {
        std::promise<void> promise;
        std::future<void> future = promise.get_future();

        boost::asio::dispatch(strand_, [&promise]() {
            promise.set_value();  // Завершаем ожидание, когда все задачи на
                                  // strand выполнены
        });

        future.wait();  // Блокируем текущий поток до завершения всех задач в
    }
};
};  // namespace impl
};  // namespace aos