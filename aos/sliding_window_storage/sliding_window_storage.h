#include <list>
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

    std::pair<bool, bool> IsDeviationRatioAboveThreshold(
        const HashT& hash_asset, const T& value,
        double given_threshold) const override {
        return deviation_tracker_->IsDeviationRatioAboveThreshold(
            hash_asset, value, given_threshold);
    }

    std::pair<bool, T> GetDeviationRatio(const HashT& hash_asset,
                                         const T& value) const override {
        return deviation_tracker_->GetDeviationRatio(hash_asset, value);
    }

    std::pair<bool, T> GetDeviationPercent(const HashT& hash_asset,
                                           const T& value) const override {
        return deviation_tracker_->GetDeviationPercent(hash_asset, value);
    }

    std::pair<bool, T> GetDeviationRatioAbs(const HashT& hash_asset,
                                            const T& value) const override {
        return deviation_tracker_->GetDeviationRatioAbs(hash_asset, value);
    }

    std::pair<bool, T> GetDeviationPercentAbs(const HashT& hash_asset,
                                              const T& value) const override {
        return deviation_tracker_->GetDeviationPercentAbs(hash_asset, value);
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

template <typename HashT, typename T>
class SlidingWindowStorageObservable : public SlidingWindowStorage<HashT, T>,
                                       public IObservable<HashT, T> {
  public:
    explicit SlidingWindowStorageObservable(
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
        : aos::impl::SlidingWindowStorage<HashT, T>(window_size, avg_tracker,
                                                    deviation_tracker,
                                                    min_tracker, max_tracker) {}

    ~SlidingWindowStorageObservable() override {
        logi("{}", "SlidingWindowStorageObservable dtor");
    }

    void AddData(const HashT asset, const T& value) override {
        // auto it = strands_.find(asset);
        // if (it == strands_.end()) {
        //     it = strands_
        //              .emplace(asset,
        //                       boost::asio::strand<
        //                           boost::asio::thread_pool::executor_type>(
        //                           thread_pool_.get_executor()))
        //              .first;
        // }
        // auto& strand = it->second;

        // Запускаем наблюдателей перед добавлением
        // boost::asio::defer(strand, [this, asset, value]() {
        for (const auto& observer : before_add_) {
            observer(asset, value);
        }
        //});

        SlidingWindowStorage<HashT, T>::AddData(asset, value);

        // Запускаем наблюдателей после добавления
        // boost::asio::defer(strand, [this, asset, value]() {
        for (const auto& observer : after_add_) {
            observer(asset, value);
        }
        //});
    }
    void AddObserverBeforeAdd(
        std::function<bool(const HashT hasht, const T& value)> observer)
        override {
        before_add_.push_back(observer);
    };
    void AddObserverAfterAdd(
        std::function<bool(const HashT hasht, const T& value)> observer)
        override {
        after_add_.push_back(observer);
    };

  private:
    std::list<std::function<bool(const HashT hasht, const T& value)>>
        before_add_;
    std::list<std::function<bool(const HashT hasht, const T& value)>>
        after_add_;
};

template <typename HashT, typename T>
class SlidingWindowStorageObservableBuilder {
  public:
    explicit SlidingWindowStorageObservableBuilder(
        common::MemoryPoolNotThreadSafety<
            SlidingWindowStorageObservable<HashT, T>>& pool)
        : pool_(pool) {}

    SlidingWindowStorageObservableBuilder& SetWindowSize(int window_size) {
        window_size_ = window_size;
        return *this;
    }

    SlidingWindowStorageObservableBuilder& SetAvgTracker(
        boost::intrusive_ptr<
            aos::IAvgTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            avg_tracker) {
        avg_tracker_ = avg_tracker;
        return *this;
    }

    SlidingWindowStorageObservableBuilder& SetDeviationTracker(
        boost::intrusive_ptr<
            aos::IDeviationTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            deviation_tracker) {
        deviation_tracker_ = deviation_tracker;
        return *this;
    }

    SlidingWindowStorageObservableBuilder& SetMinTracker(
        boost::intrusive_ptr<
            aos::IMinTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            min_tracker) {
        min_tracker_ = min_tracker;
        return *this;
    }

    SlidingWindowStorageObservableBuilder& SetMaxTracker(
        boost::intrusive_ptr<
            aos::IMaxTracker<HashT, T, common::MemoryPoolNotThreadSafety>>
            max_tracker) {
        max_tracker_ = max_tracker;
        return *this;
    }

    boost::intrusive_ptr<SlidingWindowStorageObservable<HashT, T>> Build() {
        auto* obj =
            pool_.Allocate(window_size_, avg_tracker_, deviation_tracker_,
                           min_tracker_, max_tracker_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<SlidingWindowStorageObservable<HashT, T>>(
            obj);
    }

  private:
    common::MemoryPoolNotThreadSafety<SlidingWindowStorageObservable<HashT, T>>&
        pool_;
    int window_size_ = 100;  // Значение по умолчанию
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

struct SlidingWindowStorageFactory {
    common::MemoryPoolNotThreadSafety<aos::impl::HistogramCalculator<double>>
        hc_pool{2};
    common::MemoryPoolNotThreadSafety<
        aos::impl::JointHistogramCalculator<double>>
        jhc_pool{2};
    common::MemoryPoolNotThreadSafety<
        aos::impl::MutualInformationCalculator<size_t, double>>
        mi_pool{2};
    common::MemoryPoolNotThreadSafety<
        aos::impl::SlidingWindowStorageObservable<std::size_t, double>>
        sw_pool{2};
    common::MemoryPoolNotThreadSafety<aos::impl::MarketTripletManager<size_t>>
        mt_pool{2};
    common::MemoryPoolNotThreadSafety<aos::impl::AvgTracker<size_t, double>>
        avg_pool{2};
    common::MemoryPoolNotThreadSafety<
        aos::impl::DeviationTracker<size_t, double>>
        deviation_pool{2};
    common::MemoryPoolNotThreadSafety<aos::impl::MinTracker<size_t, double>>
        min_pool{2};
    common::MemoryPoolNotThreadSafety<aos::impl::MaxTracker<size_t, double>>
        max_pool{2};

    auto Create() {
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
            aos::impl::DeviationTrackerBuilder<std::size_t, double>(
                deviation_pool)
                .SetAvgTracker(avg_tracker)
                .build();
        auto minimum_tracker =
            aos::impl::MinTrackerBuilder<std::size_t, double>(min_pool).build();
        auto maximum_tracker =
            aos::impl::MaxTrackerBuilder<std::size_t, double>(max_pool).build();

        auto market_triplet_manager =
            aos::impl::MarketTripletManager<size_t>::Create(mt_pool);
        auto sliding_window_storage =
            aos::impl::SlidingWindowStorageObservableBuilder<std::size_t,
                                                             double>(sw_pool)
                .SetWindowSize(5)
                .SetDeviationTracker(deviation_tracker)
                .SetMinTracker(minimum_tracker)
                .SetMaxTracker(maximum_tracker)
                .SetAvgTracker(avg_tracker)
                .Build();
        return std::make_tuple(
            std::move(sliding_window_storage), std::move(mi_calculator),
            std::move(deviation_tracker), std::move(market_triplet_manager));
    }
};
};  // namespace impl
};  // namespace aos