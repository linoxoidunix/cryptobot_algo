#pragma once
#include <deque>

#include "aos/common/ref_counted.h"

namespace aos {
// 📌 Интерфейс хранилища данных (Liskov Substitution - L)
template <typename HashT, typename T>
class IMaxAble {
  public:
    virtual std::pair<bool, T> GetMax(const HashT& hash_asset) const = 0;
    virtual ~IMaxAble()                                              = default;
};

template <typename HashT, typename T>
class MaxTrackerInterface : public IMaxAble<HashT, T> {
  public:
    virtual void OnAdd(const HashT& hash_asset, const T& value)    = 0;
    virtual void OnRemove(const HashT& hash_asset, const T& value) = 0;
    ~MaxTrackerInterface() override                                = default;
};

template <typename HashT, typename T, template <typename> class MemoryPool>
class IMaxTracker
    : public common::RefCounted<MemoryPool, IMaxTracker<HashT, T, MemoryPool>>,
      public IMaxAble<HashT, T> {
  public:
    virtual void OnAdd(const HashT& hash_asset, const T& value)    = 0;
    virtual void OnRemove(const HashT& hash_asset, const T& value) = 0;
    ~IMaxTracker() override                                        = default;
};

};  // namespace aos