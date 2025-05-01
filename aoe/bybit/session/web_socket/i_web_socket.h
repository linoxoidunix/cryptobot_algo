#pragma once
#include "aoe/session/web_socket/i_web_socket.h"

#pragma once
#include "boost/intrusive_ptr.hpp"
#include "concurrentqueue.h"
#include "nlohmann/json.hpp"

namespace aoe {
namespace bybit {

class WebSocketPrivateSessionWInterface
    : public WebSocketSessionWritableInterface {
  public:
    virtual ~WebSocketPrivateSessionWInterface() = default;
};

class WebSocketPrivateSessionRWInterface
    : public WebSocketPrivateSessionWInterface,
      public WebSocketSessionReadableInterface {
  public:
    virtual ~WebSocketPrivateSessionRWInterface() = default;
};

class WebSocketPublicSessionWInterface
    : public WebSocketSessionWritableInterface {
  public:
    virtual ~WebSocketPublicSessionWInterface() = default;
};

class WebSocketPublicSessionRWInterface
    : public WebSocketPublicSessionWInterface,
      public WebSocketSessionReadableInterface {
  public:
    virtual ~WebSocketPublicSessionRWInterface() = default;
};

};  // namespace bybit
};  // namespace aoe