#pragma once
#include <cstddef>

namespace aoe {
namespace binance {

constexpr std::size_t kMaximumDiffEventsFromExchange     = 10000;
constexpr std::size_t kMaximumSnapshotEventsFromExchange = 10;

/// \brief Initial number of ExecutionEvent objects preallocated in the memory
/// pool.
///
/// This value defines how many ExecutionEvent instances are initially reserved
/// in the pool to reduce dynamic heap allocations during runtime.
constexpr std::size_t kInitialExecutionEventPoolSize     = 200;
}  // namespace binance
}  // namespace aoe
