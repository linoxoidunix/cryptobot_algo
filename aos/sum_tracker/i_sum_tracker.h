#pragma once
#include <deque>

#include "aos/common/ref_counted.h"

namespace aos {
// 📌 Интерфейс хранилища данных (Liskov Substitution - L)
template <typename HashT, typename T, template <typename> class MemoryPool>
class ISumTracker
    : public common::RefCounted<MemoryPool, ISumTracker<HashT, T, MemoryPool>> {
  public:
    virtual void OnAdd(const HashT hash_asset, const T& value)       = 0;
    virtual void OnRemove(const HashT hash_asset, const T& value)    = 0;
    virtual std::pair<bool, T> GetSum(const HashT& hash_asset) const = 0;
    virtual ~ISumTracker()                                           = default;
};
};  // namespace aos