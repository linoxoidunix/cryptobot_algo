#pragma once
#include "boost/intrusive_ptr.hpp"
#include "concurrentqueue.h"
#include "nlohmann/json.hpp"

namespace aoe {
class SessionInterface {
  public:
    virtual void StartAsync()   = 0;
    virtual void StopAsync()    = 0;
    virtual ~SessionInterface() = default;
};

class WebSocketSessionWritableInterface : public SessionInterface {
  public:
    virtual void AsyncWrite(nlohmann::json&&)     = 0;
    ~WebSocketSessionWritableInterface() override = default;
};

class WebSocketSessionReadableInterface : public SessionInterface {
  public:
    virtual moodycamel::ConcurrentQueue<std::vector<char>>&
    GetResponseQueue()                            = 0;
    ~WebSocketSessionReadableInterface() override = default;
};
};  // namespace aoe