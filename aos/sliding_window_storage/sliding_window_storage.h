#include <unordered_map>

#include "aos/sliding_window_storage/i_sliding_window_storage.h"
#include "aot/common/mem_pool.h"

namespace aos {
namespace impl {
// Updated SlidingWindowStorage class
template <typename HashT, typename T>
class SlidingWindowStorage
    : public aos::ISlidingWindowStorage<HashT, T,
                                        common::MemoryPoolNotThreadSafety> {
  public:
    explicit SlidingWindowStorage(
        int window_size,
        boost::intrusive_ptr<
            aos::IAvgTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            avg_tracker,
        boost::intrusive_ptr<
            aos::IDeviationTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            deviation_tracker,
        boost::intrusive_ptr<
            aos::IMinTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            min_tracker,
        boost::intrusive_ptr<
            aos::IMaxTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            max_tracker)
        : window_size_(window_size),
          avg_tracker_(avg_tracker),
          deviation_tracker_(deviation_tracker),
          min_tracker_(min_tracker),
          max_tracker_(max_tracker) {}

    ~SlidingWindowStorage() { logi("{}", "SlidingWindowStorage dtor"); }

    void AddData(const HashT asset, const T& value) override {
        auto& window = data_[asset];
        window.emplace_back(value);

        UpdateTrackersOnAdd(asset, value);

        if (window.size() > window_size_) {
            auto front_value = window.front();
            window.pop_front();
            UpdateTrackersOnRemove(asset, front_value);
        }
    }

    const std::deque<T>& GetData(const HashT& hash_asset) const override {
        return data_.at(hash_asset);
    }

    std::pair<bool, T> GetAvg(const HashT& hash_asset) const override {
        return avg_tracker_->GetAvg(hash_asset);
    }

    std::pair<bool, T> GetDeviation(const HashT& hash_asset,
                                    const T& value) const override {
        return deviation_tracker_->GetDeviation(hash_asset, value);
    }

    std::pair<bool, bool> IsDeviationAboveThreshold(
        const HashT& hash_asset, const T& value,
        double given_threshold) const override {
        return deviation_tracker_->IsDeviationAboveThreshold(hash_asset, value,
                                                             given_threshold);
    }

    bool HasEnoughData(const HashT& hash_asset) const override {
        return data_.count(hash_asset) &&
               data_.at(hash_asset).size() >= window_size_;
    }

    std::pair<bool, T> GetMin(const HashT& hash_asset) const override {
        return min_tracker_->GetMin(hash_asset);
    };

    std::pair<bool, T> GetMax(const HashT& hash_asset) const override {
        return max_tracker_->GetMax(hash_asset);
    };

  private:
    int window_size_;
    std::unordered_map<HashT, std::deque<T>> data_;
    boost::intrusive_ptr<
        aos::IAvgTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
        avg_tracker_;
    boost::intrusive_ptr<
        aos::IDeviationTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
        deviation_tracker_;
    boost::intrusive_ptr<
        aos::IMinTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
        min_tracker_;
    boost::intrusive_ptr<
        aos::IMaxTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
        max_tracker_;
    void UpdateTrackersOnAdd(const HashT& asset, const T& value) {
        avg_tracker_->OnAdd(asset, value);
        min_tracker_->OnAdd(asset, value);
        max_tracker_->OnAdd(asset, value);
        // deviation_tracker_->OnAdd(asset, value); // Раскомментировать, если
        // необходимо
    }

    void UpdateTrackersOnRemove(const HashT& asset, const T& value) {
        avg_tracker_->OnRemove(asset, value);
        min_tracker_->OnRemove(asset, value);
        max_tracker_->OnRemove(asset, value);
        // deviation_tracker_->OnRemove(asset, value); // Раскомментировать,
        // если необходимо
    }
};
// Builder Class for SlidingWindowStorage
template <typename HashT, typename T>
class SlidingWindowStorageBuilder {
  public:
    explicit SlidingWindowStorageBuilder(
        common::MemoryPoolNotThreadSafety<SlidingWindowStorage<HashT, T>>& pool)
        : pool_(pool) {}

    SlidingWindowStorageBuilder& SetWindowSize(int window_size) {
        window_size_ = window_size;
        return *this;
    }

    SlidingWindowStorageBuilder& SetAvgTracker(
        boost::intrusive_ptr<
            aos::IAvgTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            avg_tracker) {
        avg_tracker_ = avg_tracker;
        return *this;
    }

    SlidingWindowStorageBuilder& SetDeviationTracker(
        boost::intrusive_ptr<
            aos::IDeviationTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            deviation_tracker) {
        deviation_tracker_ = deviation_tracker;
        return *this;
    }

    SlidingWindowStorageBuilder& SetMinTracker(
        boost::intrusive_ptr<
            aos::IMinTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            min_tracker) {
        min_tracker_ = min_tracker;
        return *this;
    }

    SlidingWindowStorageBuilder& SetMaxTracker(
        boost::intrusive_ptr<
            aos::IMaxTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            max_tracker) {
        max_tracker_ = max_tracker;
        return *this;
    }

    boost::intrusive_ptr<SlidingWindowStorage<HashT, T>> build() {
        auto* obj =
            pool_.Allocate(window_size_, avg_tracker_, deviation_tracker_,
                           min_tracker_, max_tracker_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<SlidingWindowStorage<HashT, T>>(obj);
    }

  private:
    common::MemoryPoolNotThreadSafety<SlidingWindowStorage<HashT, T>>& pool_;
    int window_size_ = 100;  // Default value
    boost::intrusive_ptr<
        aos::IAvgTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
        avg_tracker_;
    boost::intrusive_ptr<
        aos::IDeviationTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
        deviation_tracker_;
    boost::intrusive_ptr<
        aos::IMinTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
        min_tracker_;
    boost::intrusive_ptr<
        aos::IMaxTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
        max_tracker_;
};
};  // namespace impl
};  // namespace aos