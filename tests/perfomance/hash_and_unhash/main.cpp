#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/hash_utils/hash_utils.h"
#include "aos/common/exchange_id.h"
#include "aos/common/network_environment.h"
#include "aos/hash_utils/decompose_hash.h"
#include "aos/trading_pair/trading_pair.h"
#include "benchmark/benchmark.h"  // NOLINT

namespace {

// Фикстура для тестирования
struct HashTestFixture {
    aoe::bybit::Category market = aoe::bybit::Category::kSpot;
    aos::NetworkEnvironment net = aos::NetworkEnvironment::kMainNet;
    aos::TradingPair pair       = aos::TradingPair::kBTCUSDT;
    common::ExchangeId exchange = common::ExchangeId::kBybit;
    uint64_t hash_value         = 0;
};

}  // namespace
namespace {
// Тест хеширования
void bm_hash_key(benchmark::State& state) {
    HashTestFixture fixture;
    for (auto _ : state) {
        benchmark::DoNotOptimize(
            fixture.hash_value =
                aoe::bybit::HashKey(fixture.market, fixture.net, fixture.pair));
    }
}

// Тест декомпозиции
void bm_decompose_hashsh(benchmark::State& state) {
    HashTestFixture fixture;
    fixture.hash_value =
        aoe::bybit::HashKey(fixture.market, fixture.net, fixture.pair);

    aos::CategoryRaw out_market;
    aos::NetworkEnvironment out_net;
    aos::TradingPair out_pair;
    common::ExchangeId out_exchange;

    for (auto _ : state) {
        aos::DecomposeHash(fixture.hash_value, out_exchange, out_market,
                           out_net, out_pair);
        benchmark::DoNotOptimize(out_market);
        benchmark::DoNotOptimize(out_net);
        benchmark::DoNotOptimize(out_pair);
        benchmark::DoNotOptimize(out_exchange);
    }
}

// Комбинированный тест хеширования + декомпозиции
void bm_hash_and_decompose(benchmark::State& state) {
    HashTestFixture fixture;

    aos::CategoryRaw out_market;
    aos::NetworkEnvironment out_net;
    aos::TradingPair out_pair;
    common::ExchangeId out_exchange;

    for (auto _ : state) {
        fixture.hash_value =
            aoe::bybit::HashKey(fixture.market, fixture.net, fixture.pair);
        aos::DecomposeHash(fixture.hash_value, out_exchange, out_market,
                           out_net, out_pair);
        benchmark::DoNotOptimize(fixture.hash_value);
        benchmark::DoNotOptimize(out_market);
        benchmark::DoNotOptimize(out_net);
        benchmark::DoNotOptimize(out_pair);
        benchmark::DoNotOptimize(out_exchange);
    }
}
}  // namespace
BENCHMARK(bm_hash_key)->Unit(benchmark::kNanosecond);
BENCHMARK(bm_decompose_hashsh)->Unit(benchmark::kNanosecond);
BENCHMARK(bm_hash_and_decompose)->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();