#pragma once
#include "aos/common/ref_counted.h"
namespace aos {
template <template <typename> typename MemoryPool>
class OrderTypeInterface
    : public common::RefCounted<MemoryPool, OrderTypeInterface<MemoryPool>> {
  public:
    virtual ~OrderTypeInterface() = default;
};
};  // namespace aos