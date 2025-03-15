#include <benchmark/benchmark.h>

#include <memory>
#include <random>
#include <vector>

#include "aos/histogram/histogram_calculator.h"
#include "aos/joint_histogram/joint_histogram_calculator.h"
#include "aos/mutual_information/mutual_information_calculator.h"
#include "aos/mutual_information_real_time/mutual_information_real_time.h"

// static void BM_MutualInformation(benchmark::State& state) {
//     auto hist_calculator = std::make_shared<HistogramCalculator<double>>();
//     auto joint_hist_calculator =
//         std::make_shared<JointHistogramCalculator<double>>();
//     auto mi_calculator =
//     std::make_shared<MutualInformationCalculator<double>>(
//         hist_calculator, joint_hist_calculator);
//     RealTimeMutualInformation<double> rtmi(5, 10, mi_calculator);

//     std::vector<std::pair<double, double>> incoming_data = {
//         {29300.25, 29290.10}, {29350.50, 29340.35}, {29400.75, 29380.20},
//         {29450.00, 29430.15}, {29500.25, 29488.05}, {29550.50, 29520.40},
//         {29602.75, 29560.60}, {29000.00, 29600.50}, {29700.25, 29650.40},
//         {29750.50, 29700.35}};

//     for (auto _ : state) {
//         state.PauseTiming();
//         RealTimeMutualInformation<double> rtmi(5, 10, mi_calculator);
//         state.ResumeTiming();

//         for (const auto& data : incoming_data) {
//             rtmi.AddDataPoint(data.first, data.second);
//         }
//     }
// }

// BENCHMARK(BM_MutualInformation);

constexpr int kDataSize = 100;  // Количество элементов в данных
constexpr int kBins     = 10;   // Количество бинов

// Функция для генерации случайных данных
std::deque<double> GenerateRandomData(int size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 100.0);

    std::deque<double> data(size);
    for (auto& val : data) {
        val = dist(gen);
    }
    return data;
}

// Глобальный объект данных, чтобы избежать накладных расходов генерации
static std::deque<double> data = GenerateRandomData(kDataSize);

// Бенчмарк для ComputeHistogram с вычислением min/max
static void BM_ComputeHistogram_AutoMinMax(benchmark::State& state) {
    aos::impl::HistogramCalculator<double> calculator;
    for (auto _ : state) {
        benchmark::DoNotOptimize(calculator.ComputeHistogram(data, kBins));
    }
}
BENCHMARK(BM_ComputeHistogram_AutoMinMax);

// Бенчмарк для ComputeHistogram с переданными min/max
static void BM_ComputeHistogram_ManualMinMax(benchmark::State& state) {
    aos::impl::HistogramCalculator<double> calculator;
    double min_val = *std::min_element(data.begin(), data.end());
    double max_val = *std::max_element(data.begin(), data.end());

    for (auto _ : state) {
        benchmark::DoNotOptimize(
            calculator.ComputeHistogram(data, min_val, max_val, kBins));
    }
}
BENCHMARK(BM_ComputeHistogram_ManualMinMax);

int main(int argc, char** argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
