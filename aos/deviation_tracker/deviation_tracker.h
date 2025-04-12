#pragma once

#include <unordered_map>

#include "aos/deviation_tracker/i_deviation_tracker.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"

namespace aos {
namespace impl {

template <typename HashT, typename T>
class DeviationTrackerDefault : public DeviationTrackerInterface<HashT, T> {
    AvgTrackerInterface<HashT, T>& avg_tracker_;

  public:
    explicit DeviationTrackerDefault(AvgTrackerInterface<HashT, T>& avg_tracker)
        : avg_tracker_(avg_tracker) {}
    std::pair<bool, T> GetDeviation(const HashT& hash_asset,
                                    const T& value) const override {
        auto [status, avg] = avg_tracker_.GetAvg(hash_asset);
        if (!status) return {false, T{}};
        return {true, value - avg};
    }
    std::pair<bool, bool> IsDeviationRatioAboveThreshold(
        const HashT& hash_asset, const T& value,
        double given_threshold) const override {
        auto [status, avg] = avg_tracker_.GetAvg(hash_asset);
        if (!status) return {false, false};
        // Проверка на среднее значение 0
        if (avg == T(0)) {
            // Например, если среднее равно 0, возвращаем false
            return {false, false};
        }

        // Рассчитываем отклонение относительно среднего
        double deviation = value - avg;

        // Рассчитываем отклонение как процент от среднего
        double threshold = std::abs(deviation) / avg;

        // Сравниваем отклонение с порогом
        return {true, threshold > given_threshold};
    }

    std::pair<bool, T> GetDeviationRatio(const HashT& hash_asset,
                                         const T& value) const override {
        auto [status, avg] = avg_tracker_.GetAvg(hash_asset);
        if (!status || avg == T{0}) return {false, T{}};
        return {true, (value - avg) / avg};
    }

    std::pair<bool, T> GetDeviationPercent(const HashT& hash_asset,
                                           const T& value) const override {
        auto [status, avg] = avg_tracker_.GetAvg(hash_asset);
        if (!status || avg == T{0}) return {false, T{}};
        return {true, (value - avg) / avg * 100};
    }

    std::pair<bool, T> GetDeviationRatioAbs(const HashT& hash_asset,
                                            const T& value) const override {
        auto [status, avg] = avg_tracker_.GetAvg(hash_asset);
        if (!status || avg == T{0}) return {false, T{}};
        return {true, std::abs((value - avg)) / std::abs(avg)};
    }

    std::pair<bool, T> GetDeviationPercentAbs(const HashT& hash_asset,
                                              const T& value) const override {
        auto [status, avg] = avg_tracker_.GetAvg(hash_asset);
        if (!status || avg == T{0}) return {false, T{}};
        return {true, std::abs(value - avg) / std::abs(avg) * 100};
    }

    ~DeviationTrackerDefault() override {
        logi("{}", "DeviationTrackerDefault dtor");
    };
};

template <typename HashT, typename T>
class DeviationTracker
    : public aos::IDeviationTracker<HashT, T,
                                    common::MemoryPoolNotThreadSafety> {
  public:
    explicit DeviationTracker(
        boost::intrusive_ptr<
            aos::IAvgTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            avg_tracker)
        : avg_tracker_(avg_tracker) {}
    std::pair<bool, T> GetDeviation(const HashT& hash_asset,
                                    const T& value) const override {
        auto [status, avg] = avg_tracker_->GetAvg(hash_asset);
        if (!status) return {false, T{}};
        return {true, value - avg};
    }
    std::pair<bool, bool> IsDeviationRatioAboveThreshold(
        const HashT& hash_asset, const T& value,
        double given_threshold) const override {
        auto [status, avg] = avg_tracker_->GetAvg(hash_asset);
        if (!status) return {false, false};
        // Проверка на среднее значение 0
        if (avg == T(0)) {
            // Например, если среднее равно 0, возвращаем false
            return {false, false};
        }

        // Рассчитываем отклонение относительно среднего
        double deviation = value - avg;

        // Рассчитываем отклонение как процент от среднего
        double threshold = std::abs(deviation) / avg;

        // Сравниваем отклонение с порогом
        return {true, threshold > given_threshold};
    }

    std::pair<bool, T> GetDeviationRatio(const HashT& hash_asset,
                                         const T& value) const override {
        auto [status, avg] = avg_tracker_->GetAvg(hash_asset);
        if (!status || avg == T{0}) return {false, T{}};
        return {true, (value - avg) / avg};
    }

    std::pair<bool, T> GetDeviationPercent(const HashT& hash_asset,
                                           const T& value) const override {
        auto [status, avg] = avg_tracker_->GetAvg(hash_asset);
        if (!status || avg == T{0}) return {false, T{}};
        return {true, (value - avg) / avg * 100};
    }

    std::pair<bool, T> GetDeviationRatioAbs(const HashT& hash_asset,
                                            const T& value) const override {
        auto [status, avg] = avg_tracker_->GetAvg(hash_asset);
        if (!status || avg == T{0}) return {false, T{}};
        return {true, std::abs((value - avg)) / std::abs(avg)};
    }

    std::pair<bool, T> GetDeviationPercentAbs(const HashT& hash_asset,
                                              const T& value) const override {
        auto [status, avg] = avg_tracker_->GetAvg(hash_asset);
        if (!status || avg == T{0}) return {false, T{}};
        return {true, std::abs(value - avg) / std::abs(avg) * 100};
    }

    ~DeviationTracker() override { logi("{}", "DeviationTracker dtor"); };

  private:
    boost::intrusive_ptr<
        aos::IAvgTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
        avg_tracker_;
};

template <typename HashT, typename T>
class DeviationTrackerBuilder {
  public:
    explicit DeviationTrackerBuilder(
        common::MemoryPoolNotThreadSafety<DeviationTracker<HashT, T>>& pool)
        : pool_(pool) {}

    // Строим объект AvgTracker с заданным пулом памяти
    boost::intrusive_ptr<DeviationTracker<HashT, T>> build() {
        auto* obj = pool_.Allocate(avg_tracker_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<DeviationTracker<HashT, T>>(obj);
    }
    DeviationTrackerBuilder& SetAvgTracker(
        boost::intrusive_ptr<
            aos::IAvgTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            avg_tracker) {
        avg_tracker_ = avg_tracker;
        return *this;
    }

  private:
    common::MemoryPoolNotThreadSafety<DeviationTracker<HashT, T>>&
        pool_;  // Пул памяти
    boost::intrusive_ptr<
        aos::IAvgTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
        avg_tracker_;
};
};  // namespace impl
};  // namespace aos