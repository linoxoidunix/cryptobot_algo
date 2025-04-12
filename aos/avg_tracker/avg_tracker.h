#pragma once
#include <unordered_map>

#include "aos/avg_tracker/i_avg_tracker.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"
namespace aos {
namespace impl {

template <typename HashT, typename T>
class AvgTrackerDefault : public AvgTrackerInterface<HashT, T> {
  public:
    void OnAdd(const HashT hash_asset, const T& value) override {
        sums_[hash_asset]   += value;
        counts_[hash_asset] += 1;
        logi("Add {} for {}, new sum: {}", value, hash_asset,
             sums_[hash_asset]);
    };
    void OnRemove(const HashT hash_asset, const T& value) override {
        sums_[hash_asset]   -= value;
        counts_[hash_asset] -= 1;
        logi("Remove {} for {}, new sum: {}", value, hash_asset,
             sums_[hash_asset]);
    };
    std::pair<bool, T> GetAvg(const HashT& hash_asset) const override {
        auto it = counts_.find(hash_asset);
        if (it != counts_.end() && it->second > 0) {
            T avg = sums_.at(hash_asset) / it->second;  // Вычисляем среднее
            return {true, avg};
        }
        return {false,
                T{}};  // Если данных нет или count == 0, возвращаем false
    }
    ~AvgTrackerDefault() override { logi("{}", "AvgTrackerDefault dtor"); };

  private:
    std::unordered_map<HashT, T> sums_;
    std::unordered_map<HashT, size_t> counts_;  // Хранение количества элементов
};

template <typename HashT, typename T>
class AvgTracker
    : public aos::IAvgTracker<HashT, T, common::MemoryPoolNotThreadSafety> {
  public:
    void OnAdd(const HashT hash_asset, const T& value) override {
        sums_[hash_asset]   += value;
        counts_[hash_asset] += 1;
        logi("Add {} for {}, new sum: {}", value, hash_asset,
             sums_[hash_asset]);
    };
    void OnRemove(const HashT hash_asset, const T& value) override {
        sums_[hash_asset]   -= value;
        counts_[hash_asset] -= 1;
        logi("Remove {} for {}, new sum: {}", value, hash_asset,
             sums_[hash_asset]);
    };
    std::pair<bool, T> GetAvg(const HashT& hash_asset) const override {
        auto it = counts_.find(hash_asset);
        if (it != counts_.end() && it->second > 0) {
            T avg = sums_.at(hash_asset) / it->second;  // Вычисляем среднее
            return {true, avg};
        }
        return {false,
                T{}};  // Если данных нет или count == 0, возвращаем false
    }
    ~AvgTracker() override { logi("{}", "AvgTracker dtor"); };

  private:
    std::unordered_map<HashT, T> sums_;
    std::unordered_map<HashT, size_t> counts_;  // Хранение количества элементов
};

template <typename HashT, typename T>
class AvgTrackerBuilder {
  public:
    explicit AvgTrackerBuilder(
        common::MemoryPoolNotThreadSafety<AvgTracker<HashT, T>>& pool)
        : pool_(pool) {}

    // Строим объект AvgTracker с заданным пулом памяти
    boost::intrusive_ptr<AvgTracker<HashT, T>> build() {
        auto* obj = pool_.Allocate();
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<AvgTracker<HashT, T>>(obj);
    }

  private:
    common::MemoryPoolNotThreadSafety<AvgTracker<HashT, T>>&
        pool_;  // Пул памяти
};
};  // namespace impl
};  // namespace aos