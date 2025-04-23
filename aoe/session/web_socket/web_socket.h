#pragma once
#include "aoe/session/web_socket/i_web_socket.h"

namespace aoe {
namespace impl {
template <template <typename> class MemoryPool>
class WebSocketSession : public WebSocketSessionInterface<MemoryPool> {
  public:
    virtual void AsyncWrite(nlohmann::json&&) = 0;
    virtual ~WebSocketSessionInterface()      = default;
};
};  // namespace impl
};  // namespace aoe