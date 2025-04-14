#pragma once
#include "aos/common/ref_counted.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class ExecutionEventInterface
    : public common::RefCounted<MemoryPool, ExecutionEventInterface> {
    virtual ~ExecutionEventInterface() = default;
};
};  // namespace bybit
};  // namespace aoe