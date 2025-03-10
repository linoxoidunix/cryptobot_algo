#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <map>
#include <memory>

// Интерфейс для вычисления гистограммы
class IHistogramCalculator {
  public:
    virtual ~IHistogramCalculator() = default;
    virtual std::map<int, double> ComputeHistogram(
        const std::deque<double>& data, int bins) = 0;
};

// Интерфейс для вычисления совместной гистограммы
class IJointHistogramCalculator {
  public:
    virtual ~IJointHistogramCalculator() = default;
    virtual std::map<std::pair<int, int>, double> ComputeJointHistogram(
        const std::deque<double>& x, const std::deque<double>& y, int bins) = 0;
};

// Интерфейс для вычисления взаимной информации
class IMutualInformationCalculator {
  public:
    virtual ~IMutualInformationCalculator()           = default;
    virtual double ComputeMutualInformation(const std::deque<double>& x,
                                            const std::deque<double>& y,
                                            int bins) = 0;
};

// Стратегия для вычисления гистограммы
class HistogramCalculator : public IHistogramCalculator {
  public:
    std::map<int, double> ComputeHistogram(const std::deque<double>& data,
                                           int bins) override {
        std::map<int, double> histogram;
        double min_val   = *std::min_element(data.begin(), data.end());
        double max_val   = *std::max_element(data.begin(), data.end());
        double bin_width = (max_val - min_val) / bins;

        for (double value : data) {
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

// Стратегия для вычисления совместной гистограммы
class JointHistogramCalculator : public IJointHistogramCalculator {
  public:
    std::map<std::pair<int, int>, double> ComputeJointHistogram(
        const std::deque<double>& x, const std::deque<double>& y,
        int bins) override {
        std::map<std::pair<int, int>, double> joint_histogram;
        double min_x       = *std::min_element(x.begin(), x.end());
        double max_x       = *std::max_element(x.begin(), x.end());
        double min_y       = *std::min_element(y.begin(), y.end());
        double max_y       = *std::max_element(y.begin(), y.end());

        double bin_width_x = (max_x - min_x) / bins;
        double bin_width_y = (max_y - min_y) / bins;

        for (size_t i = 0; i < x.size(); ++i) {
            int bin_x = (x[i] - min_x) / bin_width_x;
            int bin_y = (y[i] - min_y) / bin_width_y;
            joint_histogram[{bin_x, bin_y}]++;
        }

        // Нормируем на общее число элементов
        for (auto& kv : joint_histogram) {
            kv.second /= x.size();
        }

        return joint_histogram;
    }
};

// Стратегия для вычисления взаимной информации
class MutualInformationCalculator : public IMutualInformationCalculator {
  public:
    MutualInformationCalculator(
        std::shared_ptr<IHistogramCalculator> hist_calculator,
        std::shared_ptr<IJointHistogramCalculator> joint_hist_calculator)
        : hist_calculator_(std::move(hist_calculator)),
          joint_hist_calculator_(std::move(joint_hist_calculator)) {}

    double ComputeMutualInformation(const std::deque<double>& x,
                                    const std::deque<double>& y,
                                    int bins) override {
        auto hist_x = hist_calculator_->ComputeHistogram(x, bins);
        auto hist_y = hist_calculator_->ComputeHistogram(y, bins);
        auto joint_hist =
            joint_hist_calculator_->ComputeJointHistogram(x, y, bins);

        double mi = 0.0;
        for (const auto& kv : joint_hist) {
            int bin_x   = kv.first.first;
            int bin_y   = kv.first.second;
            double p_xy = kv.second;
            double p_x  = hist_x[bin_x];
            double p_y  = hist_y[bin_y];

            if (p_xy > 0 && p_x > 0 && p_y > 0) {
                mi += p_xy * std::log2(p_xy / (p_x * p_y));
            }
        }

        return mi;
    }

  private:
    std::shared_ptr<IHistogramCalculator> hist_calculator_;
    std::shared_ptr<IJointHistogramCalculator> joint_hist_calculator_;
};

class RealTimeMutualInformation {
  public:
    RealTimeMutualInformation(
        int window_size, int bins,
        std::shared_ptr<IMutualInformationCalculator> mi_calculator)
        : window_size_(window_size),
          bins_(bins),
          mi_calculator_(std::move(mi_calculator)) {}

    void AddDataPoint(double x, double y) {
        x_window_.push_back(x);
        y_window_.push_back(y);

        if (x_window_.size() > window_size_) {
            x_window_.pop_front();
            y_window_.pop_front();
        }

        if (x_window_.size() == window_size_) {
            double mi = mi_calculator_->ComputeMutualInformation(
                x_window_, y_window_, bins_);
            std::cout << "Mutual Information (current window): " << mi
                      << std::endl;
        }
    }

  private:
    int window_size_;
    int bins_;
    std::deque<double> x_window_;
    std::deque<double> y_window_;
    std::shared_ptr<IMutualInformationCalculator> mi_calculator_;
};