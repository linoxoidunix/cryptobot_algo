#include <benchmark/benchmark.h>  // NOLINT

#include <execution>
#include <memory>
#include <random>
#include <ranges>
#include <span>
#include <vector>

#include "aos/histogram/histogram_calculator.h"
#include "aos/joint_histogram/joint_histogram_calculator.h"
#include "aos/min_tracker/min_tracker.h"
#include "aos/mutual_information/mutual_information_calculator.h"
#include "aos/mutual_information_real_time/mutual_information_real_time.h"
namespace {
// Глобальный вектор для тестов (чтобы избежать учета времени создания вектора)
std::vector<int> vec(1'000'000, 42);

// 1. Итерация через указатели
void BM_RawPointers(benchmark::State& state) {
    for (auto _ : state) {
        for (int *it = vec.data(), *end = it + vec.size(); it != end; ++it) {
            *it += 1;
        }
        benchmark::ClobberMemory();  // Чтобы компилятор не удалил цикл
    }
}

// 2. Итерация через std::ranges::for_each + unseq
void BM_RangesForEachUnseq(benchmark::State& state) {
    for (auto _ : state) {
        std::ranges::for_each(vec, [](int& x) { x += 1; });
        benchmark::ClobberMemory();
    }
}

// 3. Итерация через std::span
void BM_Span(benchmark::State& state) {
    for (auto _ : state) {
        std::span<int> sp = vec;
        for (int& x : sp) {
            x += 1;
        }
        benchmark::ClobberMemory();
    }
}

// 4. Обычный range-based for через std::vector
void BM_StdVectorRangeFor(benchmark::State& state) {
    for (auto _ : state) {
        for (int& x : vec) {
            x += 1;
        }
        benchmark::ClobberMemory();
    }
}
}  // namespace

BENCHMARK(BM_RawPointers);
BENCHMARK(BM_RangesForEachUnseq);
BENCHMARK(BM_Span);
BENCHMARK(BM_StdVectorRangeFor);

// Define the types for testing

int main(int argc, char** argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
