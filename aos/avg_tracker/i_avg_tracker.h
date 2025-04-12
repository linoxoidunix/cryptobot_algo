#pragma once
#include <deque>

#include "aos/common/ref_counted.h"

namespace aos {
// 📌 Интерфейс хранилища данных (Liskov Substitution - L)
template <typename HashT, typename T>
class IAvgAble {
  public:
    virtual std::pair<bool, T> GetAvg(const HashT& hash_asset) const = 0;
    virtual ~IAvgAble()                                              = default;
};

template <typename HashT, typename T>
class AvgTrackerInterface : public IAvgAble<HashT, T> {
  public:
    virtual void OnAdd(const HashT hash_asset, const T& value)    = 0;
    virtual void OnRemove(const HashT hash_asset, const T& value) = 0;
    virtual ~AvgTrackerInterface()                                = default;
};

template <typename HashT, typename T, template <typename> class MemoryPool>
class IAvgTracker
    : public common::RefCounted<MemoryPool, IAvgTracker<HashT, T, MemoryPool>>,
      public IAvgAble<HashT, T> {
  public:
    virtual void OnAdd(const HashT hash_asset, const T& value)    = 0;
    virtual void OnRemove(const HashT hash_asset, const T& value) = 0;
    virtual ~IAvgTracker()                                        = default;
};

};  // namespace aos