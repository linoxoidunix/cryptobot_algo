#pragma once
#include "aos/common/ref_counted.h"
#include "nlohmann/json.hpp"
namespace aos {
template <template <typename> typename MemoryPool>
class RequestInterface
    : public common::RefCounted<MemoryPool, RequestInterface<MemoryPool>> {
  public:
    virtual ~RequestInterface() = default;
};
};  // namespace aos