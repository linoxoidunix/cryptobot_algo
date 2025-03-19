#pragma once

#include <unordered_map>

#include "aos/joint_histogram/i_joint_histogram_calculator.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"

// Custom hash function for std::pair<int, int>
namespace aos {
namespace impl {
/**
 * @brief Class for calculating the joint histogram of two data sets.
 *
 * This class implements the `IJointHistogramCalculator` interface to compute
 * the joint histogram of two input data sets `x` and `y`. The histogram is
 * computed by dividing the data ranges into specified bins along each axis. The
 * resulting joint histogram represents the frequency distribution of the paired
 * values from both data sets.
 *
 * The joint histogram is normalized by dividing each bin's count by the total
 * number of elements in the input data.
 *
 * @tparam T The type of the data (e.g., `double`, `int`).
 */
template <class T>
class JointHistogramCalculator
    : public aos::IJointHistogramCalculator<T,
                                            common::MemoryPoolNotThreadSafety> {
    // Custom hash function for std::pair<int, int>

  public:
    /**
     * @brief Computes the joint histogram of two data sets.
     *
     * This method calculates the joint histogram for two data sets `x` and `y`
     * by binning the data into the specified number of bins for each axis. The
     * histogram is stored in an `std::map` where the keys are pairs of bin
     * indices (one for `x` and one for `y`), and the values are the normalized
     * counts for each bin.
     *
     * The normalization is done by dividing each bin's count by the total
     * number of elements in the data sets.
     *
     * @param x The first data set (values for the x-axis).
     * @param y The second data set (values for the y-axis).
     * @param bins The number of bins for each axis.
     *
     * @return A map where the key is a pair of bin indices (bin_x, bin_y), and
     * the value is the normalized count of occurrences in the corresponding
     * joint bin.
     */
    std::unordered_map<std::pair<int, int>, T, pair_hash> ComputeJointHistogram(
        const std::deque<T>& x, const std::deque<T>& y,
        int bins) const override {
        std::unordered_map<std::pair<int, int>, T, pair_hash> joint_histogram;
        T min_x       = *std::min_element(x.begin(), x.end());
        T max_x       = *std::max_element(x.begin(), x.end());
        T min_y       = *std::min_element(y.begin(), y.end());
        T max_y       = *std::max_element(y.begin(), y.end());

        T bin_width_x = (max_x - min_x) / bins;
        T bin_width_y = (max_y - min_y) / bins;

        for (size_t i = 0; i < x.size(); ++i) {
            int bin_x = (x[i] - min_x) / bin_width_x;
            int bin_y = (y[i] - min_y) / bin_width_y;
            joint_histogram[{bin_x, bin_y}]++;
        }

        // Normalize by total number of elements
        for (auto& kv : joint_histogram) {
            kv.second /= x.size();
        }

        return joint_histogram;
    }

    std::unordered_map<std::pair<int, int>, T, pair_hash> ComputeJointHistogram(
        const std::deque<T>& x, T x_min_value, T x_max_value,
        const std::deque<T>& y, T y_min_value, T y_max_value,
        int bins) const override {
        std::unordered_map<std::pair<int, int>, T, pair_hash> joint_histogram;

        T bin_width_x = (x_max_value - x_min_value) / bins;
        T bin_width_y = (y_max_value - y_min_value) / bins;

        for (size_t i = 0; i < x.size(); ++i) {
            int bin_x = (x[i] - x_min_value) / bin_width_x;
            int bin_y = (y[i] - y_min_value) / bin_width_y;
            joint_histogram[{bin_x, bin_y}]++;
        }

        // Normalize by total number of elements
        for (auto& kv : joint_histogram) {
            kv.second /= x.size();
        }

        return joint_histogram;
    }
    static boost::intrusive_ptr<JointHistogramCalculator<T>> Create(
        common::MemoryPoolNotThreadSafety<JointHistogramCalculator<T>>& pool) {
        auto* obj = pool.Allocate();
        obj->SetMemoryPool(&pool);
        return boost::intrusive_ptr<JointHistogramCalculator<T>>(obj);
    }
    ~JointHistogramCalculator() override {
        logi("{}", "JointHistogramCalculator dtor");
    }
};
};  // namespace impl
};  // namespace aos
