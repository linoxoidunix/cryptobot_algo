#include <deque>
#include <unordered_map>

/**
 * @brief Interface for computing histograms.
 *
 * This interface defines a method for computing histograms from input data.
 *
 * @tparam T The type of data in the histogram (e.g., double, unsigned int).
 * @details The method computes a histogram with `bins` number of bins,
 * where `int` represents the bin indices and `T` represents the values
 * associated with those bins (e.g., frequency, density, etc.).
 */
template <typename T>
class IHistogramCalculator {
  public:
    virtual ~IHistogramCalculator() = default;

    /**
     * @brief Compute the histogram for the given data.
     *
     * This method computes a histogram for the input data, dividing it
     * into the specified number of bins.
     *
     * @param data A deque containing the input data for the histogram.
     * @param bins The number of bins to divide the data into.
     * @return An unordered map where the key is the bin index (int) and
     *         the value is the data type T (e.g., frequency, density).
     */
    virtual std::unordered_map<int, T> ComputeHistogram(
        const std::deque<T>& data, int bins) = 0;
};
