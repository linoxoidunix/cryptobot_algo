#pragma once

#include <deque>

#include "aos/common/ref_counted.h"

// Предварительное объявление интерфейса ISlidingWindowStorage
namespace aos {
template <typename HashT, typename T, template <typename> class MemoryPool>
class ISlidingWindowStorage;

template <typename HashT, typename T>
class SlidingWindowStorageAvgDevMinMaxInterface;
}  // namespace aos

namespace aos {

template <typename HashT, typename T>
class MutualInformationCalculatorInterface {
  public:
    virtual ~MutualInformationCalculatorInterface()    = default;
    virtual T ComputeMutualInformation(const std::deque<T>& x,
                                       const std::deque<T>& y,
                                       int bins) const = 0;
    virtual T ComputeMutualInformation(const std::deque<T>& x, T x_min_value,
                                       T x_max_value, const std::deque<T>& y,
                                       T y_min_value, T y_max_value,
                                       int bins) const = 0;
    virtual std::pair<bool, T> ComputeMutualInformation(
        SlidingWindowStorageAvgDevMinMaxInterface<HashT, T>& sliding_window_storage,
        HashT hash_asset, HashT paired_asset, int bins) const = 0;
};

// Интерфейс для вычисления взаимной информации
template <typename T, template <typename> class MemoryPool, typename HashT,
          template <typename> class MemoryPoolSlidingWindow>
class IMutualInformationCalculator
    : public common::RefCounted<
          MemoryPool, aos::IMutualInformationCalculator<
                          T, MemoryPool, HashT, MemoryPoolSlidingWindow>> {
  public:
    virtual ~IMutualInformationCalculator()            = default;
    virtual T ComputeMutualInformation(const std::deque<T>& x,
                                       const std::deque<T>& y,
                                       int bins) const = 0;
    virtual T ComputeMutualInformation(const std::deque<T>& x, T x_min_value,
                                       T x_max_value, const std::deque<T>& y,
                                       T y_min_value, T y_max_value,
                                       int bins) const = 0;
    virtual std::pair<bool, T> ComputeMutualInformation(
        boost::intrusive_ptr<
            ISlidingWindowStorage<HashT, T, MemoryPoolSlidingWindow>>
            window_storage,
        size_t hash_asset, size_t paired_asset, int bins) const = 0;
};
};  // namespace aos
