#pragma once
#include "aos/common/ref_counted.h"

namespace aos {
// Интерфейс для пула чисел
template <typename T, template <typename> class MemoryPool>
class INumberPool
    : public common::RefCounted<MemoryPool, aos::INumberPool<T, MemoryPool>> {
  public:
    virtual ~INumberPool()                   = default;
    virtual void ReturnToPool(T id)          = 0;
    virtual std::pair<bool, T> GetFromPool() = 0;
};
};  // namespace aos