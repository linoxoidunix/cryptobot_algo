#pragma once
#include "aoe/session/web_socket/i_web_socket.h"

#pragma once
#include "boost/intrusive_ptr.hpp"
#include "concurrentqueue.h"
#include "nlohmann/json.hpp"

namespace aoe {
namespace bybit {
class WebSocketPrivateSessionInterface
    : public WebSocketSessionWritableInterface,
      public WebSocketSessionReadableInterface {
  public:
    virtual ~WebSocketPrivateSessionInterface() = default;
};
class WebSocketPublicSessionInterface
    : public WebSocketSessionWritableInterface,
      public WebSocketSessionReadableInterface {
  public:
    virtual ~WebSocketPublicSessionInterface() = default;
};
};  // namespace bybit
};  // namespace aoe