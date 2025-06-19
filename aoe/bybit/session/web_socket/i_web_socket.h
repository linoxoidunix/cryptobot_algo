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
    ~WebSocketPrivateSessionWInterface() override = default;
};

class WebSocketPrivateSessionRWInterface
    : public WebSocketPrivateSessionWInterface,
      public WebSocketSessionReadableInterface {
  public:
    ~WebSocketPrivateSessionRWInterface() override = default;
};

class WebSocketPublicSessionWInterface
    : public WebSocketSessionWritableInterface {
  public:
    ~WebSocketPublicSessionWInterface() override = default;
};

class WebSocketPublicSessionRWInterface
    : public WebSocketPublicSessionWInterface,
      public WebSocketSessionReadableInterface {
  public:
    ~WebSocketPublicSessionRWInterface() override = default;
};

};  // namespace bybit
};  // namespace aoe