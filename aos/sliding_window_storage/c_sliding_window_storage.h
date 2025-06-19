#pragma once
#include "aos/avg_tracker/avg_tracker.h"
#include "aos/deviation_tracker/deviation_tracker.h"
#include "aos/max_tracker/max_tracker.h"
#include "aos/min_tracker/min_tracker.h"
#include "aos/sliding_window_storage/i_sliding_window_storage.h"
#include "aos/sliding_window_storage/sliding_window_storage.h"

namespace aos {
namespace impl {
template <typename HashT, typename T>
class SlidingWindowStorageAvgDevMinMax
    : public SlidingWindowStorageAvgDevMinMaxInterface<HashT, T> {
    impl::AvgTrackerDefault<HashT, T> avg_tracker_;
    impl::DeviationTrackerDefault<HashT, T> deviation_tracker_;
    impl::MinTrackerDefault<HashT, T> min_tracker_;
    impl::MaxTrackerDefault<HashT, T> max_tracker_;
    impl::SlidingWindowStorageDefault<HashT, T> sliding_window_storage_;

  public:
    explicit SlidingWindowStorageAvgDevMinMax(int window_size)
        : deviation_tracker_(avg_tracker_),
          sliding_window_storage_(window_size, avg_tracker_, deviation_tracker_,
                                  min_tracker_, max_tracker_) {}
    void AddData(const HashT& hash_asset, const T& value) override {
        sliding_window_storage_.AddData(hash_asset, value);
    }

    const std::deque<T>& GetData(const HashT& hash_asset) const override {
        return sliding_window_storage_.GetData(hash_asset);
    }

    std::pair<bool, T> GetAvg(const HashT& hash_asset) const override {
        return sliding_window_storage_.GetAvg(hash_asset);
    }

    std::pair<bool, T> GetDeviation(const HashT& hash_asset,
                                    const T& value) const override {
        return sliding_window_storage_.GetDeviation(hash_asset, value);
    }

    std::pair<bool, bool> IsDeviationRatioAboveThreshold(
        const HashT& hash_asset, const T& value,
        double given_threshold) const override {
        return sliding_window_storage_.IsDeviationRatioAboveThreshold(
            hash_asset, value, given_threshold);
    }

    std::pair<bool, T> GetDeviationRatio(const HashT& hash_asset,
                                         const T& value) const override {
        return sliding_window_storage_.GetDeviationRatio(hash_asset, value);
    }

    std::pair<bool, T> GetDeviationPercent(const HashT& hash_asset,
                                           const T& value) const override {
        return sliding_window_storage_.GetDeviationPercent(hash_asset, value);
    }

    std::pair<bool, T> GetDeviationRatioAbs(const HashT& hash_asset,
                                            const T& value) const override {
        return sliding_window_storage_.GetDeviationRatioAbs(hash_asset, value);
    }

    std::pair<bool, T> GetDeviationPercentAbs(const HashT& hash_asset,
                                              const T& value) const override {
        return sliding_window_storage_.GetDeviationPercentAbs(hash_asset,
                                                              value);
    }

    bool HasEnoughData(const HashT& hash_asset) const override {
        return sliding_window_storage_.HasEnoughData(hash_asset);
    }

    std::pair<bool, T> GetMin(const HashT& hash_asset) const override {
        return sliding_window_storage_.GetMin(hash_asset);
    };

    std::pair<bool, T> GetMax(const HashT& hash_asset) const override {
        return sliding_window_storage_.GetMax(hash_asset);
    };

    void Wait() override {}
};
};  // namespace impl
};  // namespace aos