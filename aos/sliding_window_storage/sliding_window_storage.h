#pragma once
#include <list>
#include <unordered_map>

#include "aos/executer_provider/executor_provider.h"
#include "aos/executer_provider/i_executor_provider.h"
#include "aos/sliding_window_storage/i_sliding_window_storage.h"

namespace aos {
namespace impl {
template <typename HashT, typename T>
class SlidingWindowStorageDefault
    : public aos::SlidingWindowStorageAvgDevMinMaxInterface<HashT, T> {
    int window_size_;
    std::unordered_map<HashT, std::deque<T>> data_;
    AvgTrackerInterface<HashT, T>& avg_tracker_;
    DeviationTrackerInterface<HashT, T>& deviation_tracker_;
    MinTrackerInterface<HashT, T>& min_tracker_;
    MaxTrackerInterface<HashT, T>& max_tracker_;

  public:
    explicit SlidingWindowStorageDefault(
        int window_size, AvgTrackerInterface<HashT, T>& avg_tracker,
        DeviationTrackerInterface<HashT, T>& deviation_tracker,
        MinTrackerInterface<HashT, T>& min_tracker,
        MaxTrackerInterface<HashT, T>& max_tracker)
        : window_size_(window_size),
          avg_tracker_(avg_tracker),
          deviation_tracker_(deviation_tracker),
          min_tracker_(min_tracker),
          max_tracker_(max_tracker) {}

    ~SlidingWindowStorageDefault() override {
        logi("{}", "SlidingWindowStorageDefault dtor");
    }

    void AddData(const HashT& asset, const T& value) override {
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
        return avg_tracker_.GetAvg(hash_asset);
    }

    std::pair<bool, T> GetDeviation(const HashT& hash_asset,
                                    const T& value) const override {
        return deviation_tracker_.GetDeviation(hash_asset, value);
    }

    std::pair<bool, bool> IsDeviationRatioAboveThreshold(
        const HashT& hash_asset, const T& value,
        double given_threshold) const override {
        return deviation_tracker_.IsDeviationRatioAboveThreshold(
            hash_asset, value, given_threshold);
    }

    std::pair<bool, T> GetDeviationRatio(const HashT& hash_asset,
                                         const T& value) const override {
        return deviation_tracker_.GetDeviationRatio(hash_asset, value);
    }

    std::pair<bool, T> GetDeviationPercent(const HashT& hash_asset,
                                           const T& value) const override {
        return deviation_tracker_.GetDeviationPercent(hash_asset, value);
    }

    std::pair<bool, T> GetDeviationRatioAbs(const HashT& hash_asset,
                                            const T& value) const override {
        return deviation_tracker_.GetDeviationRatioAbs(hash_asset, value);
    }

    std::pair<bool, T> GetDeviationPercentAbs(const HashT& hash_asset,
                                              const T& value) const override {
        return deviation_tracker_.GetDeviationPercentAbs(hash_asset, value);
    }

    bool HasEnoughData(const HashT& hash_asset) const override {
        return data_.contains(hash_asset) &&
               data_.at(hash_asset).size() >= window_size_;
    }

    std::pair<bool, T> GetMin(const HashT& hash_asset) const override {
        return min_tracker_.GetMin(hash_asset);
    };

    std::pair<bool, T> GetMax(const HashT& hash_asset) const override {
        return max_tracker_.GetMax(hash_asset);
    };

    void Wait() override {}

  private:
    void UpdateTrackersOnAdd(const HashT& asset, const T& value) {
        avg_tracker_.OnAdd(asset, value);
        min_tracker_.OnAdd(asset, value);
        max_tracker_.OnAdd(asset, value);
        // deviation_tracker_.OnAdd(asset, value); // Раскомментировать, если
        // необходимо
    }

    void UpdateTrackersOnRemove(const HashT& asset, const T& value) {
        avg_tracker_.OnRemove(asset, value);
        min_tracker_.OnRemove(asset, value);
        max_tracker_.OnRemove(asset, value);
        // deviation_tracker_.OnRemove(asset, value); // Раскомментировать,
        // если необходимо
    }
};

template <typename HashT, typename T,
          template <typename> typename MemoryPoolNotThreadSafety>
class SlidingWindowStorage
    : public aos::ISlidingWindowStorage<HashT, T, MemoryPoolNotThreadSafety> {
  public:
    explicit SlidingWindowStorage(
        int window_size,
        boost::intrusive_ptr<
            aos::IAvgTracker<HashT, T, MemoryPoolNotThreadSafety>>
            avg_tracker,
        boost::intrusive_ptr<
            aos::IDeviationTracker<HashT, T, MemoryPoolNotThreadSafety>>
            deviation_tracker,
        boost::intrusive_ptr<
            aos::IMinTracker<HashT, T, MemoryPoolNotThreadSafety>>
            min_tracker,
        boost::intrusive_ptr<
            aos::IMaxTracker<HashT, T, MemoryPoolNotThreadSafety>>
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

    void Wait() override {}

  private:
    int window_size_;
    std::unordered_map<HashT, std::deque<T>> data_;
    boost::intrusive_ptr<aos::IAvgTracker<HashT, T, MemoryPoolNotThreadSafety>>
        avg_tracker_;
    boost::intrusive_ptr<
        aos::IDeviationTracker<HashT, T, MemoryPoolNotThreadSafety>>
        deviation_tracker_;
    boost::intrusive_ptr<aos::IMinTracker<HashT, T, MemoryPoolNotThreadSafety>>
        min_tracker_;
    boost::intrusive_ptr<aos::IMaxTracker<HashT, T, MemoryPoolNotThreadSafety>>
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
template <typename HashT, typename T,
          template <typename> typename MemoryPoolNotThreadSafety>
class SlidingWindowStorageBuilder {
  public:
    explicit SlidingWindowStorageBuilder(
        MemoryPoolNotThreadSafety<
            SlidingWindowStorage<HashT, T, MemoryPoolNotThreadSafety>>& pool)
        : pool_(pool) {}

    SlidingWindowStorageBuilder& SetWindowSize(int window_size) {
        window_size_ = window_size;
        return *this;
    }

    SlidingWindowStorageBuilder& SetAvgTracker(
        boost::intrusive_ptr<
            aos::IAvgTracker<HashT, T, MemoryPoolNotThreadSafety>>
            avg_tracker) {
        avg_tracker_ = avg_tracker;
        return *this;
    }

    SlidingWindowStorageBuilder& SetDeviationTracker(
        boost::intrusive_ptr<
            aos::IDeviationTracker<HashT, T, MemoryPoolNotThreadSafety>>
            deviation_tracker) {
        deviation_tracker_ = deviation_tracker;
        return *this;
    }

    SlidingWindowStorageBuilder& SetMinTracker(
        boost::intrusive_ptr<
            aos::IMinTracker<HashT, T, MemoryPoolNotThreadSafety>>
            min_tracker) {
        min_tracker_ = min_tracker;
        return *this;
    }

    SlidingWindowStorageBuilder& SetMaxTracker(
        boost::intrusive_ptr<
            aos::IMaxTracker<HashT, T, MemoryPoolNotThreadSafety>>
            max_tracker) {
        max_tracker_ = max_tracker;
        return *this;
    }

    boost::intrusive_ptr<
        SlidingWindowStorage<HashT, T, MemoryPoolNotThreadSafety>>
    build() {
        auto* obj =
            pool_.Allocate(window_size_, avg_tracker_, deviation_tracker_,
                           min_tracker_, max_tracker_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<
            SlidingWindowStorage<HashT, T, MemoryPoolNotThreadSafety>>(obj);
    }

  private:
    MemoryPoolNotThreadSafety<
        SlidingWindowStorage<HashT, T, MemoryPoolNotThreadSafety>>& pool_;
    int window_size_ = 100;  // Default value
    boost::intrusive_ptr<aos::IAvgTracker<HashT, T, MemoryPoolNotThreadSafety>>
        avg_tracker_;
    boost::intrusive_ptr<
        aos::IDeviationTracker<HashT, T, MemoryPoolNotThreadSafety>>
        deviation_tracker_;
    boost::intrusive_ptr<aos::IMinTracker<HashT, T, MemoryPoolNotThreadSafety>>
        min_tracker_;
    boost::intrusive_ptr<aos::IMaxTracker<HashT, T, MemoryPoolNotThreadSafety>>
        max_tracker_;
};

template <typename HashT, typename T,
          template <typename> typename MemoryPoolNotThreadSafety>
class SlidingWindowStorageDefaultObservable
    : public SlidingWindowStorageDefault<HashT, T>,
      public IObservable<HashT, T> {
  public:
    explicit SlidingWindowStorageDefaultObservable(
        int window_size, AvgTrackerInterface<HashT, T>& avg_tracker,
        DeviationTrackerInterface<HashT, T>& deviation_tracker,
        MinTrackerInterface<HashT, T>& min_tracker,
        MaxTrackerInterface<HashT, T>& max_tracker,
        boost::asio::thread_pool& thread_pool)
        : SlidingWindowStorageDefault<HashT, T>(window_size, avg_tracker,
                                                deviation_tracker, min_tracker,
                                                max_tracker),
          thread_pool_(thread_pool),
          strand_(boost::asio::make_strand(thread_pool.get_executor())) {}
    ~SlidingWindowStorageDefaultObservable() override {
        logi("{}", "SlidingWindowStorageDefaultObservable dtor");
    }

    void AddData(const HashT asset, const T& value) override {
        logi("Add hash:{} value:{} to strand", asset, value);
        // Запускаем наблюдателей перед добавлением
        boost::asio::post(strand_, [this, asset, value]() {
            std::queue<HashT> assets_to_buy;
            std::queue<HashT> assets_to_sell;
            assets_to_buy.push(asset);
            auto begin = actions_to_buy_.begin();
            while (!assets_to_buy.empty()) {
                if (begin == actions_to_buy_.end()) {
                    logi("no registered cb found");
                    break;
                }
                size_t size = assets_to_buy.size();
                for (size_t i = 0; i < size; i++) {
                    auto front_hash = assets_to_buy.front();
                    (*begin)(assets_to_buy, front_hash, value);
                    assets_to_buy.pop();
                }
                begin = std::next(begin);
            }

            begin = actions_to_sell_.begin();
            assets_to_sell.push(asset);
            while (!assets_to_sell.empty()) {
                if (begin == actions_to_sell_.end()) {
                    logi("no registered cb found");
                    break;
                }
                size_t size = assets_to_sell.size();
                for (size_t i = 0; i < size; i++) {
                    auto front_hash = assets_to_sell.front();
                    (*begin)(assets_to_sell, front_hash, value);
                    assets_to_sell.pop();
                }
                begin = std::next(begin);
            }

            logi("Start add hash:{} value:{} to sliding window", asset, value);
            SlidingWindowStorage<HashT, T, MemoryPoolNotThreadSafety>::AddData(
                asset, value);
        });
    }
    void AddActionsToBuy(std::function<void(std::queue<HashT>& queue_to_buy,
                                            const HashT hasht, const T& value)>
                             observer) override {
        actions_to_buy_.push_back(observer);
    };
    void AddActionsToSell(std::function<void(std::queue<HashT>& queue_to_sell,
                                             const HashT hasht, const T& value)>
                              observer) override {
        actions_to_sell_.push_back(observer);
    };

    void Wait() override {
        std::promise<void> promise;
        std::future<void> future = promise.get_future();

        boost::asio::dispatch(strand_, [&promise]() {
            promise.set_value();  // Завершаем ожидание, когда все задачи на
                                  // strand выполнены
        });

        future.wait();  // Блокируем текущий поток до завершения всех задач в
    }

  private:
    std::list<std::function<void(std::queue<HashT>& queue_to_buy,
                                 const HashT hasht, const T& value)>>
        actions_to_buy_;
    std::list<std::function<void(std::queue<HashT>& queue_to_sell,
                                 const HashT hasht, const T& value)>>
        actions_to_sell_;
    boost::asio::thread_pool& thread_pool_;  // Пул потоков
    boost::asio::strand<boost::asio::thread_pool::executor_type>
        strand_;  // Strand для последовательного выполнения
    boost::intrusive_ptr<
        aos::IExecutorProvider<HashT, MemoryPoolNotThreadSafety>>
        executor_provider_;
};

template <typename HashT, typename T,
          template <typename> typename MemoryPoolNotThreadSafety>
class SlidingWindowStorageObservable
    : public SlidingWindowStorage<HashT, T, MemoryPoolNotThreadSafety>,
      public IObservable<HashT, T> {
  public:
    explicit SlidingWindowStorageObservable(
        int window_size,
        boost::intrusive_ptr<
            aos::IAvgTracker<HashT, T, MemoryPoolNotThreadSafety>>
            avg_tracker,
        boost::intrusive_ptr<
            aos::IDeviationTracker<HashT, T, MemoryPoolNotThreadSafety>>
            deviation_tracker,
        boost::intrusive_ptr<
            aos::IMinTracker<HashT, T, MemoryPoolNotThreadSafety>>
            min_tracker,
        boost::intrusive_ptr<
            aos::IMaxTracker<HashT, T, MemoryPoolNotThreadSafety>>
            max_tracker,
        boost::intrusive_ptr<
            aos::IExecutorProvider<HashT, MemoryPoolNotThreadSafety>>
            executor_provider,
        boost::asio::thread_pool& thread_pool)
        : aos::impl::SlidingWindowStorage<HashT, T, MemoryPoolNotThreadSafety>(
              window_size, avg_tracker, deviation_tracker, min_tracker,
              max_tracker),
          thread_pool_(thread_pool),
          strand_(boost::asio::make_strand(thread_pool.get_executor())),
          executor_provider_(executor_provider) {}
    ~SlidingWindowStorageObservable() override {
        logi("{}", "SlidingWindowStorageObservable dtor");
    }

    void AddData(const HashT asset, const T& value) override {
        logi("Add hash:{} value:{} to strand", asset, value);
        // Запускаем наблюдателей перед добавлением
        boost::asio::post(strand_, [this, asset, value]() {
            std::queue<HashT> assets_to_buy;
            std::queue<HashT> assets_to_sell;
            assets_to_buy.push(asset);
            auto begin = actions_to_buy_.begin();
            while (!assets_to_buy.empty()) {
                if (begin == actions_to_buy_.end()) {
                    logi("no registered cb found");
                    break;
                }
                size_t size = assets_to_buy.size();
                for (size_t i = 0; i < size; i++) {
                    auto front_hash = assets_to_buy.front();
                    (*begin)(assets_to_buy, front_hash, value);
                    assets_to_buy.pop();
                }
                begin = std::next(begin);
            }

            begin = actions_to_sell_.begin();
            assets_to_sell.push(asset);
            while (!assets_to_sell.empty()) {
                if (begin == actions_to_sell_.end()) {
                    logi("no registered cb found");
                    break;
                }
                size_t size = assets_to_sell.size();
                for (size_t i = 0; i < size; i++) {
                    auto front_hash = assets_to_sell.front();
                    (*begin)(assets_to_sell, front_hash, value);
                    assets_to_sell.pop();
                }
                begin = std::next(begin);
            }

            logi("Start add hash:{} value:{} to sliding window", asset, value);
            SlidingWindowStorage<HashT, T, MemoryPoolNotThreadSafety>::AddData(
                asset, value);
        });
    }
    void AddActionsToBuy(std::function<void(std::queue<HashT>& queue_to_buy,
                                            const HashT hasht, const T& value)>
                             observer) override {
        actions_to_buy_.push_back(observer);
    };
    void AddActionsToSell(std::function<void(std::queue<HashT>& queue_to_sell,
                                             const HashT hasht, const T& value)>
                              observer) override {
        actions_to_sell_.push_back(observer);
    };

    void Wait() override {
        std::promise<void> promise;
        std::future<void> future = promise.get_future();

        boost::asio::dispatch(strand_, [&promise]() {
            promise.set_value();  // Завершаем ожидание, когда все задачи на
                                  // strand выполнены
        });

        future.wait();  // Блокируем текущий поток до завершения всех задач в
    }

  private:
    std::list<std::function<void(std::queue<HashT>& queue_to_buy,
                                 const HashT hasht, const T& value)>>
        actions_to_buy_;
    std::list<std::function<void(std::queue<HashT>& queue_to_sell,
                                 const HashT hasht, const T& value)>>
        actions_to_sell_;
    boost::asio::thread_pool& thread_pool_;  // Пул потоков
    boost::asio::strand<boost::asio::thread_pool::executor_type>
        strand_;  // Strand для последовательного выполнения
    boost::intrusive_ptr<
        aos::IExecutorProvider<HashT, MemoryPoolNotThreadSafety>>
        executor_provider_;
};

template <typename HashT, typename T,
          template <typename> typename MemoryPoolNotThreadSafety>
class SlidingWindowStorageObservableBuilder {
  public:
    explicit SlidingWindowStorageObservableBuilder(
        MemoryPoolNotThreadSafety<SlidingWindowStorageObservable<
            HashT, T, MemoryPoolNotThreadSafety>>& pool,
        boost::asio::thread_pool& thread_pool)
        : pool_(pool), thread_pool_(thread_pool) {}

    SlidingWindowStorageObservableBuilder& SetWindowSize(int window_size) {
        window_size_ = window_size;
        return *this;
    }

    SlidingWindowStorageObservableBuilder& SetAvgTracker(
        boost::intrusive_ptr<
            aos::IAvgTracker<HashT, T, MemoryPoolNotThreadSafety>>
            avg_tracker) {
        avg_tracker_ = avg_tracker;
        return *this;
    }

    SlidingWindowStorageObservableBuilder& SetDeviationTracker(
        boost::intrusive_ptr<
            aos::IDeviationTracker<HashT, T, MemoryPoolNotThreadSafety>>
            deviation_tracker) {
        deviation_tracker_ = deviation_tracker;
        return *this;
    }

    SlidingWindowStorageObservableBuilder& SetMinTracker(
        boost::intrusive_ptr<
            aos::IMinTracker<HashT, T, MemoryPoolNotThreadSafety>>
            min_tracker) {
        min_tracker_ = min_tracker;
        return *this;
    }

    SlidingWindowStorageObservableBuilder& SetMaxTracker(
        boost::intrusive_ptr<
            aos::IMaxTracker<HashT, T, MemoryPoolNotThreadSafety>>
            max_tracker) {
        max_tracker_ = max_tracker;
        return *this;
    }

    SlidingWindowStorageObservableBuilder& SetExecutorProvider(
        boost::intrusive_ptr<
            aos::IExecutorProvider<HashT, MemoryPoolNotThreadSafety>>
            executor_provider) {
        executor_provider_ = executor_provider;
        return *this;
    }

    boost::intrusive_ptr<
        SlidingWindowStorageObservable<HashT, T, MemoryPoolNotThreadSafety>>
    Build() {
        auto* obj = pool_.Allocate(
            window_size_, avg_tracker_, deviation_tracker_, min_tracker_,
            max_tracker_, executor_provider_, thread_pool_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<SlidingWindowStorageObservable<
            HashT, T, MemoryPoolNotThreadSafety>>(obj);
    }

  private:
    MemoryPoolNotThreadSafety<
        SlidingWindowStorageObservable<HashT, T, MemoryPoolNotThreadSafety>>&
        pool_;
    int window_size_ = 100;  // Значение по умолчанию
    boost::intrusive_ptr<aos::IAvgTracker<HashT, T, MemoryPoolNotThreadSafety>>
        avg_tracker_;
    boost::intrusive_ptr<
        aos::IDeviationTracker<HashT, T, MemoryPoolNotThreadSafety>>
        deviation_tracker_;
    boost::intrusive_ptr<aos::IMinTracker<HashT, T, MemoryPoolNotThreadSafety>>
        min_tracker_;
    boost::intrusive_ptr<aos::IMaxTracker<HashT, T, MemoryPoolNotThreadSafety>>
        max_tracker_;
    boost::intrusive_ptr<
        aos::IExecutorProvider<HashT, MemoryPoolNotThreadSafety>>
        executor_provider_;
    boost::asio::thread_pool& thread_pool_;
};

// template <template <typename> typename MemoryPoolNotThreadSafety>
// struct SlidingWindowStorageFactory {
//     SlidingWindowStorageFactory(boost::asio::thread_pool& _thread_pool)
//         : thread_pool(_thread_pool) {}
//     boost::asio::thread_pool& thread_pool;
//     MemoryPoolNotThreadSafety<aos::impl::HistogramCalculator<double,
//     MemoryPoolNotThreadSafety>>
//         hc_pool{2};
//     MemoryPoolNotThreadSafety<
//         aos::impl::JointHistogramCalculator<double,
//         MemoryPoolNotThreadSafety>> jhc_pool{2};
//     MemoryPoolNotThreadSafety<
//         aos::impl::MutualInformationCalculator<size_t, double,
//         MemoryPoolNotThreadSafety>> mi_pool{2};
//     MemoryPoolNotThreadSafety<
//         aos::impl::SlidingWindowStorageObservable<std::size_t, double,
//         MemoryPoolNotThreadSafety>> sw_pool{2};
//     MemoryPoolNotThreadSafety<aos::impl::MarketTripletManager<size_t,
//     MemoryPoolNotThreadSafety>>
//         mt_pool{2};
//     MemoryPoolNotThreadSafety<aos::impl::AvgTracker<size_t, double,
//     MemoryPoolNotThreadSafety>>
//         avg_pool{2};
//     MemoryPoolNotThreadSafety<
//         aos::impl::DeviationTracker<size_t, double,
//         MemoryPoolNotThreadSafety>> deviation_pool{2};
//     MemoryPoolNotThreadSafety<aos::impl::MinTracker<size_t, double,
//     MemoryPoolNotThreadSafety>>
//         min_pool{2};
//     MemoryPoolNotThreadSafety<aos::impl::MaxTracker<size_t, double,
//     MemoryPoolNotThreadSafety>>
//         max_pool{2};
//     MemoryPoolNotThreadSafety<aos::impl::ExecutorProvider<size_t,
//     MemoryPoolNotThreadSafety>>
//         exec_prov_pool{2};
//     auto Create() {
//         auto hist_calculator =
//             aos::impl::HistogramCalculator<double,
//             MemoryPoolNotThreadSafety>::Create(hc_pool);
//         auto joint_hist_calculator =
//             aos::impl::JointHistogramCalculator<double,
//             MemoryPoolNotThreadSafety>::Create(jhc_pool);
//         auto mi_calculator =
//             aos::impl::MutualInformationCalculator<size_t, double,
//             MemoryPoolNotThreadSafety>::Create(
//                 mi_pool, hist_calculator, joint_hist_calculator);

//         auto avg_tracker =
//             aos::impl::AvgTrackerBuilder<std::size_t, double,
//             MemoryPoolNotThreadSafety>(avg_pool).build();
//         auto deviation_tracker =
//             aos::impl::DeviationTrackerBuilder<std::size_t, double,
//             MemoryPoolNotThreadSafety>(
//                 deviation_pool)
//                 .SetAvgTracker(avg_tracker)
//                 .build();
//         auto minimum_tracker =
//             aos::impl::MinTrackerBuilder<std::size_t, double,
//             MemoryPoolNotThreadSafety>(min_pool).build();
//         auto maximum_tracker =
//             aos::impl::MaxTrackerBuilder<std::size_t, double,
//             MemoryPoolNotThreadSafety>(max_pool).build();

//         auto market_triplet_manager =
//             aos::impl::MarketTripletManager<size_t,
//             MemoryPoolNotThreadSafety>::Create(mt_pool);

//         auto executor_provider =
//             aos::impl::ExecutorProviderBuilder<std::size_t,
//             MemoryPoolNotThreadSafety>(exec_prov_pool,
//                                                             thread_pool)
//                 .build();

//         auto sliding_window_storage =
//             aos::impl::SlidingWindowStorageObservableBuilder<std::size_t,
//                                                              double,
//                                                              MemoryPoolNotThreadSafety>(
//                 sw_pool, thread_pool)
//                 .SetWindowSize(5)
//                 .SetDeviationTracker(deviation_tracker)
//                 .SetMinTracker(minimum_tracker)
//                 .SetMaxTracker(maximum_tracker)
//                 .SetAvgTracker(avg_tracker)
//                 .SetExecutorProvider(executor_provider)
//                 .Build();
//         return std::make_tuple(
//             std::move(sliding_window_storage), std::move(mi_calculator),
//             std::move(deviation_tracker), std::move(market_triplet_manager));
//     }
// };
};  // namespace impl
};  // namespace aos