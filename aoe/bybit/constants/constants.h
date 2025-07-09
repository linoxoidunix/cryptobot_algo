#pragma once
#include <cstddef>

namespace aoe {
namespace bybit {

constexpr std::size_t kMaximumOrderBookEventsFromExchange = 10000;
// constexpr std::size_t kMaximumSnapshotEventsFromExchange = 10;

/// \brief Initial number of ExecutionEvent objects preallocated in the memory
/// pool.
///
/// This value defines how many ExecutionEvent instances are initially reserved
/// in the pool to reduce dynamic heap allocations during runtime.
constexpr std::size_t kInitialExecutionEventPoolSize      = 200;

// used for init funding rate event pool
constexpr std::size_t kMaximumLinearTickers               = 100000;
}  // namespace bybit
}  // namespace aoe
