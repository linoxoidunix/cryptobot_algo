#pragma once
#include "aos/common/ref_counted.h"
namespace aos {
template <template <typename> typename MemoryPool>
class OrderBookEventInterface
    : public common::RefCounted<MemoryPool,
                                OrderBookEventInterface<MemoryPool>> {
  public:
    virtual ~OrderBookEventInterface() = default;
};
};  // namespace aos