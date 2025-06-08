#pragma once
#include <boost/intrusive_ptr.hpp>
#include <cmath>

#include "aos/histogram/histogram_calculator.h"
#include "aos/joint_histogram/joint_histogram_calculator.h"
#include "aos/mutual_information/i_mutual_information_calculator.h"
#include "aos/mutual_information/mutual_information_calculator.h"
#include "fmtlog.h"

namespace aos {
namespace impl {
template <class HashT, class T>
class MutualInformationCalculator
    : public MutualInformationCalculatorInterface<HashT, T> {
    impl::HistogramCalculatorDefault<T> hist_calculator_;
    impl::JointHistogramCalculatorDefault<T> joint_hist_calculator_;
    impl::MutualInformationCalculatorDefault<HashT, T>
        mutual_information_calculator_;

  public:
    MutualInformationCalculator()
        : mutual_information_calculator_(hist_calculator_,
                                         joint_hist_calculator_) {}

    T ComputeMutualInformation(const std::deque<T>& x, const std::deque<T>& y,
                               int bins) const override {
        auto hist_x = hist_calculator_.ComputeHistogram(x, bins);
        auto hist_y = hist_calculator_.ComputeHistogram(y, bins);
        auto joint_hist =
            joint_hist_calculator_.ComputeJointHistogram(x, y, bins);

        T mi{};
        for (const auto& kv : joint_hist) {
            int bin_x = kv.first.first;
            int bin_y = kv.first.second;
            T p_xy    = kv.second;
            T p_x     = hist_x[bin_x];
            T p_y     = hist_y[bin_y];

            if (p_xy > 0 && p_x > 0 && p_y > 0) {
                mi += p_xy * std::log2(p_xy / (p_x * p_y));
            }
        }

        return mi;
    }

    T ComputeMutualInformation(const std::deque<T>& x, T x_min_value,
                               T x_max_value, const std::deque<T>& y,
                               T y_min_value, T y_max_value,
                               int bins) const override {
        auto hist_x     = hist_calculator_.ComputeHistogram(x, x_min_value,
                                                            x_max_value, bins);
        auto hist_y     = hist_calculator_.ComputeHistogram(y, y_min_value,
                                                            y_max_value, bins);
        auto joint_hist = joint_hist_calculator_.ComputeJointHistogram(
            x, x_min_value, x_max_value, y, y_min_value, y_max_value, bins);

        T mi{};
        for (const auto& kv : joint_hist) {
            int bin_x = kv.first.first;
            int bin_y = kv.first.second;
            T p_xy    = kv.second;
            T p_x     = hist_x[bin_x];
            T p_y     = hist_y[bin_y];

            if (p_xy > 0 && p_x > 0 && p_y > 0) {
                mi += p_xy * std::log2(p_xy / (p_x * p_y));
            }
        }

        return mi;
    }

    std::pair<bool, T> ComputeMutualInformation(
        SlidingWindowStorageAvgDevMinMaxInterface<HashT, T>& window_storage,
        size_t hash_asset, size_t paired_asset, int bins) const override {
        // Проверяем, есть ли достаточно данных для обоих активов
        if (!window_storage.HasEnoughData(hash_asset) ||
            !window_storage.HasEnoughData(paired_asset)) {
            return {false, T{}};  // Недостаточно данных для вычисления
        }

        // Извлекаем данные для обоих активов
        const auto& data_x              = window_storage.GetData(hash_asset);
        const auto& data_y              = window_storage.GetData(paired_asset);

        // Извлекаем минимальные и максимальные значения для каждого актива
        auto [min_status_x, min_hash_x] = window_storage.GetMin(hash_asset);
        auto [max_status_x, max_hash_x] = window_storage.GetMax(hash_asset);
        auto [min_status_y, min_hash_y] = window_storage.GetMin(paired_asset);
        auto [max_status_y, max_hash_y] = window_storage.GetMax(paired_asset);

        // Если хотя бы для одного актива нет данных, продолжаем
        if (!min_status_x || !max_status_x || !min_status_y || !max_status_y) {
            return {false, T{}};  // Нет минимальных/максимальных данных
        }

        // Вычисляем взаимную информацию
        T mi = ComputeMutualInformation(data_x, min_hash_x, max_hash_x, data_y,
                                        min_hash_y, max_hash_y, bins);

        return {true, mi};
    }

    ~MutualInformationCalculator() override {
        logi("MutualInformationCalculator dtor");
    }
};
};  // namespace impl
};  // namespace aos