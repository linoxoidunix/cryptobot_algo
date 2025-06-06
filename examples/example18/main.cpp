#include <boost/asio.hpp>

#include "aos/aos.h"
#include "fmtlog.h"

int main() {
    {
    using HashT = std::size_t;
    using Price = double;
    using Qty   = double;
    using namespace aos::impl;
    MarketTripletManagerDefault<HashT> market_triplet_manager;
    market_triplet_manager.Connect(1, 2);

    boost::asio::thread_pool thread_pool;
    AvgTrackerDefault<HashT, Price> avg_tracker_default;
    DeviationTrackerDefault<HashT, Price> deviation_tracker(
        avg_tracker_default);
    MinTrackerDefault<HashT, Price> min_tracker;
    MaxTrackerDefault<HashT, Price> max_tracker;

    SlidingWindowStorageDefault<HashT, Price> sliding_window(
        5, avg_tracker_default, deviation_tracker, min_tracker, max_tracker);

    MinTrackerDefault<HashT, Price> min_tracker_default;
    MaxTrackerDefault<HashT, Price> max_tracker_default;
    DeviationTrackerDefault<HashT, Price> deviation_tracker_default(
        avg_tracker_default);
    HistogramCalculatorDefault<Price> histogram_calculator_default;
    JointHistogramCalculatorDefault<Price> joint_histogram_calculator_default;
    MutualInformationCalculatorDefault<HashT, Price> mi_calculator_default(
        histogram_calculator_default, joint_histogram_calculator_default);
    }
    fmtlog::poll();
    return 0;
}
