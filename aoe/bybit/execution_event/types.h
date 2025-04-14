#pragma once
#include "aoe/bybit/execution_event/i_types.h"
namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class ExecutionEventDefault : public ExecutionEventInterface<MemoryPool> {
  public:
    ExecutionEventDefault() = default;
    ~ExecutionEventDefault() override {};
};
};  // namespace bybit
};  // namespace aoe