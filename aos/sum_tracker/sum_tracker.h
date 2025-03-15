#include <unordered_map>

#include "aos/sum_tracker/i_sum_tracker.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"
namespace aos {
namespace impl {
template <typename HashT, typename T>
class SumTracker
    : public aos::ISumTracker<HashT, T, common::MemoryPoolNotThreadSafety> {
  public:
    void OnAdd(const HashT hash_asset, const T& value) override {
        sums_[hash_asset] += value;
        logi("Add {} for {}, new sum: {}", value, hash_asset,
             sums_[hash_asset]);
    };
    void OnRemove(const HashT hash_asset, const T& value) override {
        sums_[hash_asset] -= value;
        logi("Remove {} for {}, new sum: {}", value, hash_asset,
             sums_[hash_asset]);
    };
    std::pair<bool, T> GetSum(const HashT& hash_asset) const override {
        auto it = sums_.find(hash_asset);
        if (it != sums_.end() && it->second != T(0)) {
            return {true, it->second};  // Возвращаем сумму, если она существует
                                        // и не равна 0
        }
        return {false,
                T(0)};  // Возвращаем false, если данных нет или сумма равна 0
    };
    ~SumTracker() override { logi("{}", "SumTracker dtor"); };

  private:
    std::unordered_map<HashT, T> sums_;
};

template <typename HashT, typename T>
class SumTrackerBuilder {
  public:
    // Конструктор принимает пул памяти, который будет использоваться для
    // выделения памяти для объекта
    explicit SumTrackerBuilder(
        common::MemoryPoolNotThreadSafety<SumTracker<HashT, T>>& pool)
        : pool_(pool) {}

    // Строим объект SumTracker
    boost::intrusive_ptr<SumTracker<HashT, T>> build() {
        auto* obj = pool_.Allocate();  // Выделяем память для объекта
        obj->SetMemoryPool(&pool_);    // Устанавливаем пул памяти
        return boost::intrusive_ptr<SumTracker<HashT, T>>(
            obj);  // Возвращаем умный указатель
    }

  private:
    common::MemoryPoolNotThreadSafety<SumTracker<HashT, T>>&
        pool_;  // Пул памяти
};
};  // namespace impl
};  // namespace aos