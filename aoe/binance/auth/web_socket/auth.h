#pragma once
#include <cstdint>

#include "aoe/auth/web_socket/i_auth.h"
#include "aoe/session/web_socket/i_web_socket.h"
#include "aoe/signer/ed25519/i_signer.h"
#include "aos/logger/mylog.h"
namespace aoe {
namespace binance {
namespace impl {
class Authentificator : public AuthInterface {
    WebSocketSessionWritableInterface& web_socket_session_;
    aoe::ed25519::SignerInterface& signer_;

  public:
    Authentificator(WebSocketSessionWritableInterface& web_socket_session,
                    aoe::ed25519::SignerInterface& signer)
        : web_socket_session_(web_socket_session), signer_(signer) {}
    void Auth() override {
        auto api_key = signer_.ApiKey();
        int64_t timestamp =
            std::time(nullptr) * 1000;  // Unix timestamp in milliseconds
        auto request =
            fmt::format("apiKey={}&timestamp={}", api_key, timestamp);
        auto [status, signature] = signer_.SignBase64EncodeStandard(request);
        if (!status) return;
        nlohmann::json message = {{"id", nullptr},  // Уникальный ID запроса
                                  {"method", "session.logon"},
                                  {"params",
                                   {{"apiKey", api_key},
                                    {"signature", std::move(signature)},
                                    {"timestamp", timestamp}}}};
        web_socket_session_.AsyncWrite(std::move(message));
    }
};
};  // namespace impl
};  // namespace binance
};  // namespace aoe