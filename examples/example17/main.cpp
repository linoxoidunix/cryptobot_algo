#include <boost/asio.hpp>
#include "fmtlog.h"
#include "fmt/ranges.h"
#include "fmt/format.h"
#include "aos/aos.h"

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

        Strategy<HashT, Price> strategy;
        SlidingWindowStorageDefault<HashT, Price> sliding_window(
            5, avg_tracker_default, deviation_tracker, min_tracker,
            max_tracker);
        // mi_calculator init
        MinTrackerDefault<HashT, Price> min_tracker_default;
        MaxTrackerDefault<HashT, Price> max_tracker_default;
        DeviationTrackerDefault<HashT, Price> deviation_tracker_default(
            avg_tracker_default);
        HistogramCalculatorDefault<Price> histogram_calculator_default;
        JointHistogramCalculatorDefault<Price>
            joint_histogram_calculator_default;
        MutualInformationCalculatorDefault<HashT, Price> mi_calculator_default(
            histogram_calculator_default, joint_histogram_calculator_default);
        // mi_calculator stop init

        strategy.AddActionsToBuy([](std::queue<size_t>& queue,
                                    const size_t hash, const double& value) {
            if (hash == 1) {
                logi("start analise hash:{} value:{}", hash, value);
                queue.push(hash);
            }
        });

        strategy.AddActionsToBuy([&sliding_window](std::queue<size_t>& queue,
                                                   const size_t hash,
                                                   const double& value) {
            auto [status_deviation_ratio, deviation_ratio] =
                sliding_window.GetDeviationRatio(hash, value);
            auto deviation = sliding_window.GetDeviation(hash, value);
            logi(
                "Before adding hash:{} value:{} deviation_ratio:{} "
                "deviation:{}",
                hash, value, deviation_ratio, deviation);
            if (deviation_ratio > 0.001) {
                queue.push(hash);
            }
        });

        strategy.AddActionsToBuy(
            [&mi_calculator_default, &sliding_window, &market_triplet_manager](
                std::queue<size_t>& queue, const size_t hash_asset,
                const double& value) {
                if (!market_triplet_manager.HasPair(hash_asset)) return;
                for (const auto& paired_asset :
                     market_triplet_manager.GetPairs(hash_asset)) {
                    auto [status, mi] =
                        mi_calculator_default.ComputeMutualInformation(
                            sliding_window, hash_asset, paired_asset, 10);
                    if (!status) continue;
                    logi("Mutual Information ({} ↔ {}):{}", hash_asset,
                         paired_asset, mi);
                    if (mi > 2) {
                        queue.push(paired_asset);
                    }
                }
            });

        strategy.AddActionsToBuy([](std::queue<size_t>& queue,
                                    const size_t hash_asset,
                                    const double& value) {
            logi("Need buy hash:{} value:{}", hash_asset, value);
        });

        strategy.AddActionsToSell([&sliding_window](std::queue<size_t>& queue,
                                                    const size_t hash,
                                                    const double& value) {
            if (hash == 1) {
                logi("start analise hash:{} value:{}", hash, value);
                queue.push(hash);
            }
        });

        strategy.AddActionsToSell([&sliding_window](std::queue<size_t>& queue,
                                                    const size_t hash,
                                                    const double& value) {
            auto [status_deviation_ratio, deviation_ratio] =
                sliding_window.GetDeviationRatio(hash, value);
            auto deviation = sliding_window.GetDeviation(hash, value);
            logi(
                "Before adding hash:{} value:{} deviation_ratio:{} "
                "deviation:{}",
                hash, value, deviation_ratio, deviation);
            if (deviation_ratio < 0.001) {
                queue.push(hash);
            }
        });

        strategy.AddActionsToSell(
            [&mi_calculator_default, &sliding_window, &market_triplet_manager](
                std::queue<size_t>& queue, const size_t hash_asset,
                const double& value) {
                if (!market_triplet_manager.HasPair(hash_asset)) return;
                for (const auto& paired_asset :
                     market_triplet_manager.GetPairs(hash_asset)) {
                    auto [status, mi] =
                        mi_calculator_default.ComputeMutualInformation(
                            sliding_window, hash_asset, paired_asset, 10);
                    if (!status) continue;
                    logi("Mutual Information ({} ↔ {}):{}", hash_asset,
                         paired_asset, mi);
                    if (mi > 2) {
                        queue.push(paired_asset);
                    }
                }
            });

        strategy.AddActionsToSell([](std::queue<size_t>& queue,
                                     const size_t hash_asset,
                                     const double& value) {
            logi("Need sell hash:{} value:{}", hash_asset, value);
            return true;
        });

        StrategyEngineDefault<HashT, Price> strategy_engine(
            thread_pool, strategy, sliding_window);
        std::vector<std::pair<double, double>> incoming_data = {
            {29300.25, 29290.10}, {29350.50, 29340.35}, {29400.75, 29380.20},
            {29450.00, 29430.15}, {29500.25, 29488.05}, {29550.50, 29520.40},
            {29602.75, 29560.60}, {29000.00, 29600.50}, {49700.25, 29650.40},
            {29750.50, 59700.35}};
        size_t hash_first_asset  = 1;
        size_t hash_second_asset = 2;
        for (size_t i = 0; i < incoming_data.size(); ++i) {
            strategy_engine.AddData(hash_first_asset, incoming_data[i].first);
            strategy_engine.AddData(hash_second_asset, incoming_data[i].second);
        }
        strategy_engine.Wait();
    }
    fmtlog::poll();
    return 0;
}