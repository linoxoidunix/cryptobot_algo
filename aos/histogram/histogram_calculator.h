#pragma once
#include <algorithm>
#include <boost/intrusive_ptr.hpp>
#include <deque>
#include <unordered_map>

#include "aos/common/ref_counted.h"
#include "aos/histogram/i_histogram_calculator.h"
#include "aos/logger/mylog.h"

namespace aos {
namespace impl {

template <typename T>
class HistogramCalculatorDefault : public HistogramCalculatorInterface<T> {
  public:
    std::unordered_map<int, T> ComputeHistogram(const std::deque<T>& data,
                                                int bins) const override {
        std::unordered_map<int, T> histogram;
        T min_val   = *std::min_element(data.begin(), data.end());
        T max_val   = *std::max_element(data.begin(), data.end());
        T bin_width = (max_val - min_val) / bins;

        for (const T& value : data) {
            T bin_index = (value - min_val) / bin_width;
            histogram[bin_index]++;
        }

        // Нормируем на общее число элементов
        for (auto& kv : histogram) {
            kv.second /= data.size();
        }

        return histogram;
    }
    virtual std::unordered_map<int, T> ComputeHistogram(
        const std::deque<T>& data, T min_val, T max_val,
        int bins) const override {
        std::unordered_map<int, T> histogram;
        T bin_width = (max_val - min_val) / bins;

        for (const T& value : data) {
            int bin_index = (value - min_val) / bin_width;
            histogram[bin_index]++;
        }

        // Нормируем на общее число элементов
        for (auto& kv : histogram) {
            kv.second /= data.size();
        }

        return histogram;
    };
    ~HistogramCalculatorDefault() override {
        logi("{}", "HistogramCalculatorDefault dtor");
    }
};
template <class T, template <typename> typename MemoryPoolNotThreadSafety>
class HistogramCalculator
    : public aos::IHistogramCalculator<T, MemoryPoolNotThreadSafety> {
  public:
    std::unordered_map<int, T> ComputeHistogram(const std::deque<T>& data,
                                                int bins) const override {
        std::unordered_map<int, T> histogram;
        T min_val   = *std::min_element(data.begin(), data.end());
        T max_val   = *std::max_element(data.begin(), data.end());
        T bin_width = (max_val - min_val) / bins;

        for (const T& value : data) {
            T bin_index = (value - min_val) / bin_width;
            histogram[bin_index]++;
        }

        // Нормируем на общее число элементов
        for (auto& kv : histogram) {
            kv.second /= data.size();
        }

        return histogram;
    }
    virtual std::unordered_map<int, T> ComputeHistogram(
        const std::deque<T>& data, T min_val, T max_val,
        int bins) const override {
        std::unordered_map<int, T> histogram;
        T bin_width = (max_val - min_val) / bins;

        for (const T& value : data) {
            int bin_index = (value - min_val) / bin_width;
            histogram[bin_index]++;
        }

        // Нормируем на общее число элементов
        for (auto& kv : histogram) {
            kv.second /= data.size();
        }

        return histogram;
    };
    static boost::intrusive_ptr<
        HistogramCalculator<T, MemoryPoolNotThreadSafety>>
    Create(MemoryPoolNotThreadSafety<
           HistogramCalculator<T, MemoryPoolNotThreadSafety>>& pool) {
        auto* obj = pool.Allocate();
        obj->SetMemoryPool(&pool);
        return boost::intrusive_ptr<
            HistogramCalculator<T, MemoryPoolNotThreadSafety>>(obj);
    }
    ~HistogramCalculator() override { logi("{}", "HistogramCalculator dtor"); }
};
};  // namespace impl
};  // namespace aos