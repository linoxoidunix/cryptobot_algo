#pragma once
#include <queue>

#include "aos/avg_tracker/i_avg_tracker.h"
#include "aos/common/common.h"
#include "aos/common/i_waitable.h"
#include "aos/common/ref_counted.h"
#include "aos/deviation_tracker/i_deviation_tracker.h"
#include "aos/max_tracker/i_max_tracker.h"
#include "aos/min_tracker/i_min_tracker.h"

namespace aos {
// 📌 Интерфейс хранилища данных (Liskov Substitution - L)
template <typename HashT, typename T>
class SlidingWindowStorageAvgDevMinMaxInterface
    : public common::IWaitable,
      public IAvgAble<HashT, T>,
      public IDeviationAble<HashT, T>,
      public IMinAble<HashT, T>,
      public IMaxAble<HashT, T> {
  public:
    virtual void AddData(const HashT& hash_asset, const T& value)       = 0;
    virtual const std::deque<T>& GetData(const HashT& hash_asset) const = 0;
    virtual bool HasEnoughData(const HashT& hash_asset) const           = 0;
    ~SlidingWindowStorageAvgDevMinMaxInterface() override = default;
};

template <typename HashT, typename T, template <typename> class MemoryPool>
class ISlidingWindowStorage
    : public common::RefCounted<MemoryPool,
                                ISlidingWindowStorage<HashT, T, MemoryPool>>,
      public common::IWaitable,
      public IAvgAble<HashT, T>,
      public IDeviationAble<HashT, T>,
      public IMinAble<HashT, T>,
      public IMaxAble<HashT, T> {
  public:
    virtual void AddData(const HashT& hash_asset, const T& value)       = 0;
    virtual const std::deque<T>& GetData(const HashT& hash_asset) const = 0;
    virtual bool HasEnoughData(const HashT& hash_asset) const           = 0;
    ~ISlidingWindowStorage() override = default;
};

// 📌 Расширенный интерфейс с поддержкой наблюдателей (OCP - открыт для
// расширения)
template <typename HashT, typename T>
class IObservable {
  public:
    virtual void AddActionsToBuy(
        std::function<void(std::queue<HashT>& queue_to_buy, const HashT hasht,
                           const T& value)>
            observer) = 0;
    virtual void AddActionsToSell(
        std::function<void(std::queue<HashT>& queue_to_sell, const HashT hasht,
                           const T& value)>
            observer)      = 0;
    virtual ~IObservable() = default;
};

};  // namespace aos