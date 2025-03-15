#pragma once

#include <deque>

#include "aos/common/ref_counted.h"

namespace aos {
// Интерфейс для вычисления взаимной информации
template <typename T, template <typename> class MemoryPool>
class IMutualInformationCalculator
    : public common::RefCounted<
          MemoryPool, aos::IMutualInformationCalculator<T, MemoryPool>> {
  public:
    virtual ~IMutualInformationCalculator() = default;
    virtual T ComputeMutualInformation(const std::deque<T>& x,
                                       const std::deque<T>& y, int bins) = 0;
    virtual T ComputeMutualInformation(const std::deque<T>& x, T x_min_value,
                                       T x_max_value, const std::deque<T>& y,
                                       T y_min_value, T y_max_value,
                                       int bins)                         = 0;
};
};  // namespace aos
