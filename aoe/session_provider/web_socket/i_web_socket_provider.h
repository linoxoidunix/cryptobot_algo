#pragma once
#include "aoe/session/web_socket/i_web_socket.h"
namespace aoe {

template <template <typename> class MemoryPool>
class WebSocketSessionProviderInterface {
  public:
    virtual ~WebSocketSessionProviderInterface()         = default;
    virtual WebSocketSessionWritableInterface& Provide() = 0;
};
};  // namespace aoe