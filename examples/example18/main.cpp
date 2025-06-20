// Copyright 2025 Denis Evlanov

#include "aos/avg_tracker/avg_tracker.h"
#include "aos/deviation_tracker/deviation_tracker.h"
#include "aos/histogram/histogram_calculator.h"
#include "aos/joint_histogram/joint_histogram_calculator.h"
#include "aos/logger/mylog.h"
#include "aos/market_triplet_manager/market_triplet_manager.h"
#include "aos/max_tracker/max_tracker.h"
#include "aos/min_tracker/min_tracker.h"
#include "aos/mutual_information/mutual_information_calculator.h"
#include "aos/sliding_window_storage/sliding_window_storage.h"

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
            aos::impl::AvgTrackerDefault<HashT, Price> avg_tracker_default;
            aos::impl::DeviationTrackerDefault<HashT, Price> deviation_tracker(
                avg_tracker_default);
            aos::impl::MinTrackerDefault<HashT, Price> min_tracker;
            aos::impl::MaxTrackerDefault<HashT, Price> max_tracker;

            aos::impl::SlidingWindowStorageDefault<HashT, Price> sliding_window(
                5, avg_tracker_default, deviation_tracker, min_tracker,
                max_tracker);

            aos::impl::MinTrackerDefault<HashT, Price> min_tracker_default;
            aos::impl::MaxTrackerDefault<HashT, Price> max_tracker_default;
            aos::impl::DeviationTrackerDefault<HashT, Price>
                deviation_tracker_default(avg_tracker_default);
            aos::impl::HistogramCalculatorDefault<Price>
                histogram_calculator_default;
            aos::impl::JointHistogramCalculatorDefault<Price>
                joint_histogram_calculator_default;
            aos::impl::MutualInformationCalculatorDefault<HashT, Price>
                mi_calculator_default(histogram_calculator_default,
                                      joint_histogram_calculator_default);
        } catch (...) {
            loge("error occured");
        }
    }
    fmtlog::poll();
    return 0;
}
