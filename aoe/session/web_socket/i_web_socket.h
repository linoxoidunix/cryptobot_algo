#pragma once
#include "boost/intrusive_ptr.hpp"
#include "nlohmann/json.hpp"
namespace aoe {

class WebSocketSessionInterface {
  public:
    virtual void AsyncWrite(nlohmann::json&&) = 0;
    virtual ~WebSocketSessionInterface()      = default;
};
};  // namespace aoe