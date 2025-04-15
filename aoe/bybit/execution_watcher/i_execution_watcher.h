#pragma once
#include <boost/intrusive_ptr.hpp>

#include "aoe/bybit/execution_event/types.h"
namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionWatcherInterface {
  public:
    virtual void OnNewExecutionEvent(
        boost::intrusive_ptr<ExecutionEventInterface<MemoryPool, PositionT>>
            event)               = 0;
    ~ExecutionWatcherInterface() = default;
};
};  // namespace bybit
};  // namespace aoe