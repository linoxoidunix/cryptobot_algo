#pragma once

#include "aoe/session_provider/web_socket/i_web_socket_provider.h"
namespace aoe {
namespace impl {
template <template <typename> class MemoryPool>
class WebSocketSessionProvider
    : public WebSocketSessionProviderInterface<MemoryPool> {
    WebSocketSessionInterface& web_socket_session_;

  public:
    WebSocketSessionProvider(WebSocketSessionInterface& web_socket_session)
        : web_socket_session_(web_socket_session) {}
    WebSocketSessionInterface& Provide() { return web_socket_session_; };
    ~WebSocketSessionProvider() override = default;
};
};  // namespace impl
};  // namespace aoe