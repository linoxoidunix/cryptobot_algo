#pragma once
#include "boost/intrusive_ptr.hpp"
#include "nlohmann/json.hpp"
namespace aoe {

template <template <typename> class MemoryPool>
class WebSocketSessionInterface {
  public:
    virtual void AsyncWrite(nlohmann::json&&) = 0;
    virtual ~WebSocketSessionInterface()      = default;
};
};  // namespace aoe