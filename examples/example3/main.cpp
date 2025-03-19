#include <iostream>
#include <vector>

#include "aos/avg_tracker/avg_tracker.h"
#include "aos/deviation_tracker/deviation_tracker.h"
#include "aos/histogram/histogram_calculator.h"
#include "aos/joint_histogram/joint_histogram_calculator.h"
#include "aos/market_triplet_manager/market_triplet_manager.h"
#include "aos/max_tracker/max_tracker.h"
#include "aos/min_tracker/min_tracker.h"
#include "aos/mutual_information/mutual_information_calculator.h"
#include "aos/mutual_information_real_time/mutual_information_real_time.h"
#include "aos/sliding_window_storage/sliding_window_storage.h"
#include "aot/common/mem_pool.h"

int main() {
    common::MemoryPoolNotThreadSafety<aos::impl::HistogramCalculator<double>>
        hc_pool(2);
    common::MemoryPoolNotThreadSafety<
        aos::impl::JointHistogramCalculator<double>>
        jhc_pool(2);
    common::MemoryPoolNotThreadSafety<
        aos::impl::MutualInformationCalculator<size_t, double>>
        mi_pool(2);
    common::MemoryPoolNotThreadSafety<
        aos::impl::SlidingWindowStorage<std::size_t, double>>
        sw_pool(2);
    common::MemoryPoolNotThreadSafety<aos::impl::MarketTripletManager<size_t>>
        mt_pool(2);
    common::MemoryPoolNotThreadSafety<aos::impl::AvgTracker<size_t, double>>
        avg_pool(2);
    common::MemoryPoolNotThreadSafety<
        aos::impl::DeviationTracker<size_t, double>>
        deviation_pool(2);
    common::MemoryPoolNotThreadSafety<aos::impl::MinTracker<size_t, double>>
        min_pool(2);
    common::MemoryPoolNotThreadSafety<aos::impl::MaxTracker<size_t, double>>
        max_pool(2);

    auto hist_calculator =
        aos::impl::HistogramCalculator<double>::Create(hc_pool);
    auto joint_hist_calculator =
        aos::impl::JointHistogramCalculator<double>::Create(jhc_pool);
    auto mi_calculator =
        aos::impl::MutualInformationCalculator<size_t, double>::Create(
            mi_pool, hist_calculator, joint_hist_calculator);
    auto avg_tracker =
        aos::impl::AvgTrackerBuilder<std::size_t, double>(avg_pool).build();
    auto deviation_tracker =
        aos::impl::DeviationTrackerBuilder<std::size_t, double>(deviation_pool)
            .SetAvgTracker(avg_tracker)
            .build();
    auto minimum_tracker =
        aos::impl::MinTrackerBuilder<std::size_t, double>(min_pool).build();
    auto maximum_tracker =
        aos::impl::MaxTrackerBuilder<std::size_t, double>(max_pool).build();

    auto sliding_window_storage =
        aos::impl::SlidingWindowStorageBuilder<size_t, double>(sw_pool)
            .SetWindowSize(5)
            .SetAvgTracker(avg_tracker)
            .SetDeviationTracker(deviation_tracker)
            .SetMinTracker(minimum_tracker)
            .SetMaxTracker(maximum_tracker)
            .build();
    auto market_triplet_manager =
        aos::impl::MarketTripletManager<size_t>::Create(mt_pool);

    market_triplet_manager->Connect(1, 2);

    boost::asio::thread_pool pool;
    aos::impl::RealTimeMutualInformation<double, size_t> rtmi(
        pool, 5, mi_calculator, market_triplet_manager, sliding_window_storage);

    // int num_points = 100;  // Генерируем 100 точек данных
    //  auto incoming_data = GenerateNonLinearData(num_points);

    // Пример поступающих данных в реальном времени
    // std::vector<std::pair<double, double>> incoming_data = {
    //     {1.2, 2.3}, {3.4, 3.1}, {2.2, 1.9}, {4.5, 4.2}, {5.1, 5.3},
    //     {3.3, 3.5}, {2.8, 2.7}, {4.1, 4.3}, {5.6, 5.7}, {3.9, 3.8}};

    std::vector<std::pair<double, double>> incoming_data = {
        {29300.25, 29290.10}, {29350.50, 29340.35}, {29400.75, 29380.20},
        {29450.00, 29430.15}, {29500.25, 29488.05}, {29550.50, 29520.40},
        {29602.75, 29560.60}, {29000.00, 29600.50}, {49700.25, 29650.40},
        {29750.50, 59700.35}};
    size_t hash_first_asset  = 1;
    size_t hash_second_asset = 2;
    // Обрабатываем данные по одному
    // int i                    = 0;
    // for (const auto& data : incoming_data) {
    //     rtmi.AddDataPoint(hash_first_asset, data.first);
    //     rtmi.AddDataPoint(hash_second_asset, data.second);
    //     // i++;
    // }
    for (const auto& data : incoming_data) {
        sliding_window_storage->AddData(hash_first_asset, data.first);
        sliding_window_storage->AddData(hash_second_asset, data.second);
        // i++;
    }

    rtmi.Block();
    // Убираем указатели
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    mi_calculator         = nullptr;
    hist_calculator       = nullptr;
    joint_hist_calculator = nullptr;
    return 0;
}