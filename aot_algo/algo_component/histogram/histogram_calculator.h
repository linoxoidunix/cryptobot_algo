#include "aot/algo_component/histogram_calcultator.h"

template <class T>
class HistogramCalculator : public IHistogramCalculator<T> {
  public:
    std::unordered_map<int, T> ComputeHistogram(const std::deque<T>& data,
                                                int bins) override {
        std::unordered_map<int, double> histogram;
        double min_val   = *std::min_element(data.begin(), data.end());
        double max_val   = *std::max_element(data.begin(), data.end());
        double bin_width = (max_val - min_val) / bins;

        for (T& value : data) {
            int bin_index = (value - min_val) / bin_width;
            histogram[bin_index]++;
        }

        // Нормируем на общее число элементов
        for (auto& kv : histogram) {
            kv.second /= data.size();
        }

        return histogram;
    }
};