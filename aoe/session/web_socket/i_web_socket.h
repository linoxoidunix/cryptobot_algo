#pragma once
#include "boost/intrusive_ptr.hpp"
#include "concurrentqueue.h"
#include "nlohmann/json.hpp"

namespace aoe {

class WebSocketSessionWritableInterface {
  public:
    virtual void AsyncWrite(nlohmann::json&&)    = 0;
    virtual ~WebSocketSessionWritableInterface() = default;
};

class WebSocketSessionReadableInterface {
  public:
    virtual moodycamel::ConcurrentQueue<std::vector<char>>&
    GetResponseQueue()                           = 0;
    virtual ~WebSocketSessionReadableInterface() = default;
};
};  // namespace aoe