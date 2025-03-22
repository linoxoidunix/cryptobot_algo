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
    virtual void AddData(const HashT hash_asset, const T& value)        = 0;
    virtual const std::deque<T>& GetData(const HashT& hash_asset) const = 0;
    virtual bool HasEnoughData(const HashT& hash_asset) const           = 0;
    virtual ~ISlidingWindowStorage() = default;
};

// 📌 Расширенный интерфейс с поддержкой наблюдателей (OCP - открыт для
// расширения)
template <typename HashT, typename T>
class IObservable {
  public:
    // virtual void AddMainObserverBeforeAdd(
    //     std::function<bool(const HashT hasht, const T& value)> observer) = 0;
    // virtual void AddMainObserverAfterAdd(
    //     std::function<bool(const HashT hasht, const T& value)> observer) = 0;
    // virtual void AddPartitialObserverBeforeAdd(
    //     std::function<bool(const HashT hasht, const T& value)> observer) = 0;
    // virtual void AddPartitialObserverAfterAdd(
    //     std::function<bool(const HashT hasht, const T& value)> observer) = 0;
    virtual void AddObserverBeforeAdd(
        std::function<void(std::queue<HashT>& queue, const HashT hasht,
                           const T& value)>
            observer) = 0;
    virtual void AddObserverAfterAdd(
        std::function<void(std::queue<HashT>& queue, const HashT hasht,
                           const T& value)>
            observer)      = 0;
    virtual ~IObservable() = default;
};

};  // namespace aos