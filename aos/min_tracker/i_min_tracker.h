#pragma once
#include <deque>

#include "aos/common/ref_counted.h"

namespace aos {
// 📌 Интерфейс хранилища данных (Liskov Substitution - L)
template <typename HashT, typename T>
class IMinAble {
  public:
    virtual std::pair<bool, T> GetMin(const HashT& hash_asset) const = 0;
    virtual ~IMinAble()                                              = default;
};

template <typename HashT, typename T, template <typename> class MemoryPool>
class IMinTracker
    : public common::RefCounted<MemoryPool, IMinTracker<HashT, T, MemoryPool>>,
      public IMinAble<HashT, T> {
  public:
    virtual void OnAdd(const HashT hash_asset, const T& value)    = 0;
    virtual void OnRemove(const HashT hash_asset, const T& value) = 0;
    virtual ~IMinTracker()                                        = default;
};

};  // namespace aos