#pragma once

#include "aoe/bybit/execution_watcher/i_execution_watcher.h"
#include "aos/position_storage/position_storage_by_pair/i_position_storage_by_pair.h"
#include "aot/Logger.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionWatcherDefault
    : ExecutionWatcherInterface<MemoryPool, PositionT> {
    aos::PositionStorageByPairInterface<double, double, PositionT>&
        position_storage_;

  public:
    ExecutionWatcherDefault(
        aos::PositionStorageByPairInterface<double, double, PositionT>&
            position_storage)
        : position_storage_(position_storage) {};
    void OnEvent(
        boost::intrusive_ptr<ExecutionEventInterface<MemoryPool, PositionT>>
            event) {
        event->Accept(position_storage_);
    };
    ~ExecutionWatcherDefault() = default;
};
};  // namespace bybit
};  // namespace aoe