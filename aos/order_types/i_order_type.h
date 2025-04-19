#pragma once
#include "aos/common/ref_counted.h"
#include "nlohmann/json.hpp"
namespace aos {
template <template <typename> typename MemoryPool>
class OrderTypeInterface
    : public common::RefCounted<MemoryPool, OrderTypeInterface<MemoryPool>> {
  public:
    virtual nlohmann::json ToJson() = 0;
    virtual ~OrderTypeInterface()   = default;
};
};  // namespace aos