#pragma once

#include <deque>
#include <unordered_map>

#include "aos/common/ref_counted.h"
namespace aos {
struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        // Combine the two integers (x and y) into a single hash value
        std::size_t hash1 =
            std::hash<int>{}(p.first);  // Hash for the first element (int)
        std::size_t hash2 =
            std::hash<int>{}(p.second);  // Hash for the second element (int)

        // Combine the two hash values using bitwise operations
        return hash1 ^ (hash2 << 1);  // XOR and shift to avoid collisions
    }
};
/**
 * @brief Interface for calculating the joint histogram of two data sets.
 *
 * This class provides a method to compute the joint histogram, which is a
 * histogram that represents the distribution of two variables (x and y) in a
 * binned manner.
 *
 * The `ComputeJointHistogram` method calculates the joint histogram based on
 * two input data sets and the number of bins for each axis.
 *
 * @tparam T The type of the data (e.g., `double`, `int`).
 */
template <typename T, template <typename> class MemoryPool>
class IJointHistogramCalculator
    : public common::RefCounted<MemoryPool,
                                aos::IJointHistogramCalculator<
                                    T, common::MemoryPoolNotThreadSafety>> {
  public:
    /**
     * @brief Virtual destructor to ensure proper cleanup in derived classes.
     */
    virtual ~IJointHistogramCalculator() = default;

    /**
     * @brief Computes the joint histogram for two data sets.
     *
     * This method computes the joint histogram of two input data sets `x` and
     * `y` by dividing each axis into the specified number of bins. The result
     * is returned as an unordered map where each key is a pair of bin indices
     * and the corresponding value is the count or frequency of occurrences in
     * that bin.
     *
     * @param x The first data set.
     * @param y The second data set.
     * @param bins The number of bins for each axis.
     *
     * @return A map where the key is a pair of bin indices (x-bin, y-bin) and
     * the value is the count of occurrences in that bin.
     */
    virtual std::unordered_map<std::pair<int, int>, T, pair_hash>
    ComputeJointHistogram(const std::deque<T>& x, const std::deque<T>& y,
                          int bins) = 0;

    virtual std::unordered_map<std::pair<int, int>, T, pair_hash>
    ComputeJointHistogram(const std::deque<T>& x, T x_min_value, T x_max_value,
                          const std::deque<T>& y, T y_min_value, T y_max_value,
                          int bins) = 0;
};
};  // namespace aos
