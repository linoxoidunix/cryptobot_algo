#pragma once
#include "aoe/auth/web_socket/i_auth.h"
#include "aoe/session/web_socket/i_web_socket.h"
#include "aoe/signer/hmac_sha256/i_signer.h"
#include "fmtlog.h"
namespace aoe {
namespace bybit {
namespace impl {
class Authentificator : public AuthInterface {
    WebSocketSessionWritableInterface& web_socket_session_;
    aoe::hmac_sha256::SignerInterface& signer_;

  public:
    Authentificator(WebSocketSessionWritableInterface& web_socket_session,
                    aoe::hmac_sha256::SignerInterface& signer)
        : web_socket_session_(web_socket_session), signer_(signer) {}
    void Auth() override {
        // https://github.com/bybit-exchange/api-usage-examples/blob/master/V5_demo/wss_demo/c%2B%2B/ws_trade_api_demo.cpp
        // wait 10ms to expire
        long expires = std::time(nullptr) * 1000 +
                       10000;  // Unix timestamp in milliseconds
        std::string request    = fmt::format("GET/realtime{}", expires);
        std::string signature  = signer_.Sign(request);
        nlohmann::json message = {
            {"op", "auth"},
            {"args", {signer_.ApiKey(), expires, std::move(signature)}}};
        web_socket_session_.AsyncWrite(std::move(message));
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe