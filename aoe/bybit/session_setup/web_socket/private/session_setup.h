#pragma once
#include "aoe/auth/web_socket/i_auth.h"
#include "aoe/bybit/auth/web_socket/auth.h"
#include "aoe/bybit/ping_manager/for_private_channel/ping_manager.h"
#include "aoe/bybit/session/web_socket/i_web_socket.h"
#include "aoe/credentials/api_key/api_key.h"
#include "aoe/credentials/secret_key/secret_key.h"
#include "aoe/credentials_loader/i_credentials_loader.h"
#include "aoe/signer/hmac_sha256/signer.h"

namespace aoe {
namespace bybit {
namespace impl {
namespace main_net {
class PrivateSessionSetup {
    WebSocketPrivateSessionWInterface& ws_;
    CredentialsLoaderInterface& credentials_loader_;
    PingManagerInterface& ping_manager_;

  public:
    PrivateSessionSetup(WebSocketPrivateSessionWInterface& ws,
                        CredentialsLoaderInterface& credentials_loader,
                        PingManagerInterface& ping_manager)
        : ws_(ws),
          credentials_loader_(credentials_loader),
          ping_manager_(ping_manager) {}

    bool Setup() {
        auto [ok1, api_key] = credentials_loader_.ApiKeyHmacSha256MainNet();
        auto [ok2, secret_key] =
            credentials_loader_.SecretKeyHmacSha256MainNet();
        if (!ok1 || !ok2) {
            return false;
        }
        aoe::impl::StaticApiKey api_key_manager(api_key);
        aoe::impl::StaticSecretKey secret_key_manager(secret_key);
        aoe::hmac_sha256::impl::Signer signer(api_key_manager,
                                              secret_key_manager);
        aoe::bybit::impl::Authentificator authentificator(ws_, signer);
        authentificator.Auth();
        ping_manager_.Start();
        return true;
    }
    virtual ~PrivateSessionSetup() = default;
};
};  // namespace main_net
namespace test_net {
class PrivateSessionSetup {
    WebSocketPrivateSessionWInterface& ws_;
    CredentialsLoaderInterface& credentials_loader_;
    PingManagerInterface& ping_manager_;

  public:
    PrivateSessionSetup(WebSocketPrivateSessionWInterface& ws,
                        CredentialsLoaderInterface& credentials_loader,
                        PingManagerInterface& ping_manager)
        : ws_(ws),
          credentials_loader_(credentials_loader),
          ping_manager_(ping_manager) {}

    bool Setup() {
        auto [ok1, api_key] = credentials_loader_.ApiKeyHmacSha256TestNet();
        auto [ok2, secret_key] =
            credentials_loader_.SecretKeyHmacSha256TestNet();
        if (!ok1 || !ok2) {
            return false;
        }
        aoe::impl::StaticApiKey api_key_manager(api_key);
        aoe::impl::StaticSecretKey secret_key_manager(secret_key);
        aoe::hmac_sha256::impl::Signer signer(api_key_manager,
                                              secret_key_manager);
        aoe::bybit::impl::Authentificator authentificator(ws_, signer);
        authentificator.Auth();
        ping_manager_.Start();
        return true;
    }
    virtual ~PrivateSessionSetup() = default;
};
};  // namespace test_net
};  // namespace impl
};  // namespace bybit
};  // namespace aoe
