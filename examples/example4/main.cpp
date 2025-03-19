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
    aos::impl::SlidingWindowStorageFactory factory;
    auto [sliding_window_storage, mi_calculator, deviation_tracker,
          market_triplet_manager] = factory.Create();

    market_triplet_manager->Connect(1, 2);

    sliding_window_storage->AddObserverBeforeAdd(
        [&sliding_window_storage](const size_t hash, const double& value) {
            auto deviation_ratio =
                sliding_window_storage->GetDeviationRatio(hash, value);
            auto deviation = sliding_window_storage->GetDeviation(hash, value);
            logi(
                "Before adding hash:{} value:{} deviation_ratio:{} "
                "deviation:{}",
                hash, value, deviation_ratio, deviation);
            return true;
        });

    sliding_window_storage->AddObserverAfterAdd(
        [](const size_t hash, const double& value) {
            logi("After adding hash:{} value:{}", hash, value);
            return true;
        });

    sliding_window_storage->AddObserverAfterAdd(
        [&mi_calculator, &sliding_window_storage, &market_triplet_manager](
            const size_t hash_asset, const double& value) {
            if (!market_triplet_manager->HasPair(hash_asset)) return false;
            for (const auto& paired_asset :
                 market_triplet_manager->GetPairs(hash_asset)) {
                auto [status, mi] = mi_calculator->ComputeMutualInformation(
                    sliding_window_storage, hash_asset, paired_asset, 10);
                if (!status) continue;
                logi("Mutual Information ({} ↔ {}):{}", hash_asset,
                     paired_asset, mi);
            }
            return true;
        });
    std::vector<std::pair<double, double>> incoming_data = {
        {29300.25, 29290.10}, {29350.50, 29340.35}, {29400.75, 29380.20},
        {29450.00, 29430.15}, {29500.25, 29488.05}, {29550.50, 29520.40},
        {29602.75, 29560.60}, {29000.00, 29600.50}, {49700.25, 29650.40},
        {29750.50, 59700.35}};
    size_t hash_first_asset  = 1;
    size_t hash_second_asset = 2;

    for (const auto& data : incoming_data) {
        sliding_window_storage->AddData(hash_first_asset, data.first);
        sliding_window_storage->AddData(hash_second_asset, data.second);
        // i++;
    }

    // Убираем указатели
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    // mi_calculator                 = nullptr;
    // hist_calculator               = nullptr;
    // joint_hist_calculator         = nullptr;
    return 0;
}