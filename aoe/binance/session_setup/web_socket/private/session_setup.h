#pragma once
#include "aoe/auth/web_socket/i_auth.h"
#include "aoe/binance/auth/web_socket/auth.h"
#include "aoe/binance/session/web_socket/i_web_socket.h"
#include "aoe/credentials/api_key/api_key.h"
#include "aoe/credentials/secret_key/ed25519/url_safety_no_padding/secret_key.h"
#include "aoe/credentials_loader/i_credentials_loader.h"
#include "aoe/signer/ed25519/signer.h"

namespace aoe {
namespace binance {
namespace impl {
namespace main_net {
class PrivateSessionSetup {
    WebSocketPrivateSessionWInterface& ws_;
    CredentialsLoaderInterface& credentials_loader_;

  public:
    PrivateSessionSetup(WebSocketPrivateSessionWInterface& ws,
                        CredentialsLoaderInterface& credentials_loader)
        : ws_(ws), credentials_loader_(credentials_loader) {}

    bool Setup() {
        auto [ok1, api_key] = credentials_loader_.ApiKeyEd25519MainNet();
        auto [ok2, secret_key] =
            credentials_loader_
                .SecretKeyEd25519MainNetBase64URLSafetyNoPadding();
        if (!ok1 || !ok2) {
            return false;
        }
        aoe::impl::StaticApiKey api_key_manager(api_key);
        aoe::ed25519::impl::StaticSecretKeyBase64URLSafetyNoPadding
            secret_key_manager(secret_key);
        aoe::ed25519::impl::Signer signer(api_key_manager, secret_key_manager);
        Authentificator authentificator(ws_, signer);
        authentificator.Auth();
        return true;
    }
    virtual ~PrivateSessionSetup() = default;
};
};  // namespace main_net
namespace test_net {
class PrivateSessionSetup {
    WebSocketPrivateSessionWInterface& ws_;
    CredentialsLoaderInterface& credentials_loader_;

  public:
    PrivateSessionSetup(WebSocketPrivateSessionWInterface& ws,
                        CredentialsLoaderInterface& credentials_loader)
        : ws_(ws), credentials_loader_(credentials_loader) {}

    bool Setup() {
        auto [ok1, api_key] = credentials_loader_.ApiKeyHmacSha256TestNet();
        auto [ok2, secret_key] =
            credentials_loader_.SecretKeyHmacSha256TestNet();
        if (!ok1 || !ok2) {
            return false;
        }
        aoe::impl::StaticApiKey api_key_manager(api_key);
        aoe::ed25519::impl::StaticSecretKeyBase64URLSafetyNoPadding
            secret_key_manager(secret_key);
        aoe::ed25519::impl::Signer signer(api_key_manager, secret_key_manager);
        aoe::binance::impl::Authentificator authentificator(ws_, signer);
        authentificator.Auth();
        return true;
    }
    virtual ~PrivateSessionSetup() = default;
};
};  // namespace test_net
};  // namespace impl
};  // namespace binance
};  // namespace aoe
