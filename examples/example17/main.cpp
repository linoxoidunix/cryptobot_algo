#include <boost/asio.hpp>

#include "aos/aos.h"

int main() {
    using HashT = std::size_t;
    using Price = double;
    using Qty   = double;
    using namespace aos::impl;
    boost::asio::thread_pool thread_pool;
    AvgTrackerDefault<HashT, Price> avg_tracker_default;
    DeviationTrackerDefault<HashT, Price> deviation_tracker(
        avg_tracker_default);
    MinTrackerDefault<HashT, Price> min_tracker;
    MaxTrackerDefault<HashT, Price> max_tracker;

    // Strategy<HashT, Price> strategy;

    // StrategyEngineDefault<std::size_t, double> strategy_engine(
    //     thread_pool, strategy, )
    return 0;
}