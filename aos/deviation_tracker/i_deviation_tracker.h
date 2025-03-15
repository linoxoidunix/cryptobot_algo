#pragma once
#include <deque>

#include "aos/common/ref_counted.h"

namespace aos {
template <typename HashT, typename T>
class IDeviationAble {
  public:
    virtual std::pair<bool, T> GetDeviation(const HashT& hash_asset,
                                            const T& value) const = 0;
    virtual std::pair<bool, bool> IsDeviationAboveThreshold(
        const HashT& hash_asset, const T& value,
        double given_threshold) const = 0;
    virtual ~IDeviationAble()         = default;
};
// 📌 Интерфейс хранилища данных (Liskov Substitution - L)
template <typename HashT, typename T, template <typename> class MemoryPool>
class IDeviationTracker
    : public common::RefCounted<MemoryPool,
                                IDeviationTracker<HashT, T, MemoryPool>>,
      public IDeviationAble<HashT, T> {
  public:
    virtual ~IDeviationTracker() = default;
};
};  // namespace aos