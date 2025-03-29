#pragma once
#include <boost/intrusive_ptr.hpp>
#include <cmath>

#include "aos/common/ref_counted.h"
#include "aos/histogram/i_histogram_calculator.h"
#include "aos/joint_histogram/i_joint_histogram_calculator.h"
#include "aos/mutual_information/i_mutual_information_calculator.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"

namespace aos {
namespace impl {
// Стратегия для вычисления взаимной информации
template <class HashT, class T>
class MutualInformationCalculator
    : public aos::IMutualInformationCalculator<
          T, common::MemoryPoolNotThreadSafety, HashT,
          common::MemoryPoolNotThreadSafety> {
  public:
    MutualInformationCalculator(
        boost::intrusive_ptr<
            aos::IHistogramCalculator<T, common::MemoryPoolNotThreadSafety>>
            hist_calculator,
        boost::intrusive_ptr<aos::IJointHistogramCalculator<
            T, common::MemoryPoolNotThreadSafety>>
            joint_hist_calculator)
        : hist_calculator_(hist_calculator),
          joint_hist_calculator_(joint_hist_calculator) {}

    T ComputeMutualInformation(const std::deque<T>& x, const std::deque<T>& y,
                               int bins) const override {
        auto hist_x = hist_calculator_->ComputeHistogram(x, bins);
        auto hist_y = hist_calculator_->ComputeHistogram(y, bins);
        auto joint_hist =
            joint_hist_calculator_->ComputeJointHistogram(x, y, bins);

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
        auto hist_x     = hist_calculator_->ComputeHistogram(x, x_min_value,
                                                             x_max_value, bins);
        auto hist_y     = hist_calculator_->ComputeHistogram(y, y_min_value,
                                                             y_max_value, bins);
        auto joint_hist = joint_hist_calculator_->ComputeJointHistogram(
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
        boost::intrusive_ptr<
            ISlidingWindowStorage<HashT, T, common::MemoryPoolNotThreadSafety>>
            window_storage,
        size_t hash_asset, size_t paired_asset, int bins) const override {
        // Проверяем, есть ли достаточно данных для обоих активов
        if (!window_storage->HasEnoughData(hash_asset) ||
            !window_storage->HasEnoughData(paired_asset)) {
            return {false, T{}};  // Недостаточно данных для вычисления
        }

        // Извлекаем данные для обоих активов
        const auto& data_x              = window_storage->GetData(hash_asset);
        const auto& data_y              = window_storage->GetData(paired_asset);

        // Извлекаем минимальные и максимальные значения для каждого актива
        auto [min_status_x, min_hash_x] = window_storage->GetMin(hash_asset);
        auto [max_status_x, max_hash_x] = window_storage->GetMax(hash_asset);
        auto [min_status_y, min_hash_y] = window_storage->GetMin(paired_asset);
        auto [max_status_y, max_hash_y] = window_storage->GetMax(paired_asset);

        // Если хотя бы для одного актива нет данных, продолжаем
        if (!min_status_x || !max_status_x || !min_status_y || !max_status_y) {
            return {false, T{}};  // Нет минимальных/максимальных данных
        }

        // Вычисляем взаимную информацию
        T mi = ComputeMutualInformation(data_x, min_hash_x, max_hash_x, data_y,
                                        min_hash_y, max_hash_y, bins);

        return {true, mi};
    }

    static boost::intrusive_ptr<MutualInformationCalculator<HashT, T>> Create(
        common::MemoryPoolNotThreadSafety<
            MutualInformationCalculator<HashT, T>>& pool,
        boost::intrusive_ptr<
            IHistogramCalculator<T, common::MemoryPoolNotThreadSafety>>
            hist_calculator,
        boost::intrusive_ptr<
            IJointHistogramCalculator<T, common::MemoryPoolNotThreadSafety>>
            joint_hist_calculator) {
        auto* obj = pool.Allocate(hist_calculator, joint_hist_calculator);
        obj->SetMemoryPool(&pool);
        return boost::intrusive_ptr<MutualInformationCalculator<HashT, T>>(obj);
    }
    ~MutualInformationCalculator() override {
        logi("MutualInformationCalculator dtor");
    }

  private:
    boost::intrusive_ptr<
        aos::IHistogramCalculator<T, common::MemoryPoolNotThreadSafety>>
        hist_calculator_;
    boost::intrusive_ptr<
        aos::IJointHistogramCalculator<T, common::MemoryPoolNotThreadSafety>>
        joint_hist_calculator_;
};
};  // namespace impl
};  // namespace aos