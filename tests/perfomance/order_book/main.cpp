#include <benchmark/benchmark.h>

#include "aos/trading_pair/trading_pair.h"
#include "aot/Logger.h"
#include "aot/common/types.h"
#include "aot/strategy/market_order_book.h"

static void BM_MarketOrderBook2_Update(benchmark::State& state) {
    Trading::MarketOrderBook2 order_book(common::ExchangeId::kBinance,
                                         common::TradingPair{2, 1});

    std::vector<std::unique_ptr<Exchange::MEMarketUpdate2>> updates;
    updates.reserve(
        state.range(0));  // Количество обновлений для каждой итерации

    for (int i = 0; i < state.range(0); ++i) {
        auto update   = std::make_unique<Exchange::MEMarketUpdate2>();
        update->price = 10000 + (i % 100);
        update->qty   = 1 + (i % 10);
        update->side  = (i % 2 == 0) ? common::Side::kBid : common::Side::kAsk;
        updates.push_back(std::move(update));
    }

    double total_time_ns  = 0.0;
    size_t total_elements = updates.size();  // Общее количество элементов

    for (auto _ : state) {
        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& update : updates) {
            order_book.OnMarketUpdate(update.get());
        }
        auto end = std::chrono::high_resolution_clock::now();
        total_time_ns +=
            std::chrono::duration<double, std::nano>(end - start).count();
    }

    // Общее количество итераций и элементов
    double avg_time_per_iter =
        total_time_ns / state.iterations();  // Среднее время на одну итерацию
    double avg_time_per_element =
        total_time_ns /
        (state.iterations() * total_elements);  // Среднее время на элемент

    state.counters["AvgTimePerIter(ns)"]    = avg_time_per_iter;
    state.counters["AvgTimePerElement(ns)"] = avg_time_per_element;
}

BENCHMARK(BM_MarketOrderBook2_Update)->Arg(1000)->Arg(10000)->Arg(100000);

int main(int argc, char** argv) {
    fmtlog::setLogLevel(fmtlog::OFF);
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
