#pragma once

#include "aoe/bybit/execution_watcher/i_execution_watcher.h"
#include "aos/position/position.h"
#include "aos/position_storage/position_storage_by_pair/i_position_storage_by_pair.h"
#include "aot/Logger.h"

namespace aoe {
namespace bybit {
namespace impl {
template <template <typename> typename MemoryPool, typename Price = double,
          typename Qty       = double,

          typename PositionT = aos::impl::NetPositionDefault<Price, Qty>>
class ExecutionWatcherNetDefault
    : ExecutionWatcherInterface<MemoryPool, PositionT> {
    aos::PositionStorageByPairInterface<Price, Qty, PositionT>&
        position_storage_;

  public:
    ExecutionWatcherNetDefault(
        aos::PositionStorageByPairInterface<Price, Qty, PositionT>&
            position_storage)
        : position_storage_(position_storage) {};
    void OnEvent(
        boost::intrusive_ptr<ExecutionEventInterface<MemoryPool, PositionT>>
            event) {
        event->Accept(position_storage_);
    };
    ~ExecutionWatcherNetDefault() = default;
};

template <template <typename> typename MemoryPool, typename Price = double,
          typename Qty       = double,

          typename PositionT = aos::impl::HedgedPositionDefault<Price, Qty>>
class ExecutionWatcherHedgedDefault
    : ExecutionWatcherInterface<MemoryPool, PositionT> {
    aos::PositionStorageByPairInterface<Price, Qty, PositionT>&
        position_storage_;

  public:
    ExecutionWatcherHedgedDefault(
        aos::PositionStorageByPairInterface<Price, Qty, PositionT>&
            position_storage)
        : position_storage_(position_storage) {};
    void OnEvent(
        boost::intrusive_ptr<ExecutionEventInterface<MemoryPool, PositionT>>
            event) {
        event->Accept(position_storage_);
    };
    ~ExecutionWatcherHedgedDefault() = default;
};

template <template <typename> typename MemoryPool, typename PositionT,
          typename Price = double, typename Qty = double>
class ExecutionWatcherDefault
    : ExecutionWatcherInterface<MemoryPool, PositionT> {
    aos::PositionStorageByPairInterface<Price, Qty, PositionT>&
        position_storage_;

  public:
    ExecutionWatcherDefault(
        aos::PositionStorageByPairInterface<Price, Qty, PositionT>&
            position_storage)
        : position_storage_(position_storage) {};
    void OnEvent(
        boost::intrusive_ptr<ExecutionEventInterface<MemoryPool, PositionT>>
            event) {
        event->Accept(position_storage_);
    };
    ~ExecutionWatcherDefault() = default;
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe