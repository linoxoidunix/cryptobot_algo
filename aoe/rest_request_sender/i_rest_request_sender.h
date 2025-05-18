#pragma once
#include "aos/request/i_request.h"
namespace aoe {
template <template <typename> typename MemoryPool>
class RestRequestSenderInterface {
  public:
    virtual ~RestRequestSenderInterface()                         = default;
    virtual void Send(aos::RequestInterface<MemoryPool>& request) = 0;
};
};  // namespace aoe