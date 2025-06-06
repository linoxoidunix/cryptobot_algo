#include <thread>

#include "aoe/aoe.h"
#include "aoe/credentials/api_key/api_key.h"
#include "aoe/credentials/secret_key/secret_key.h"
#include "aoe/signer/hmac_sha256/signer.h"
#include "aos/aos.h"
#include "fmtlog.h"

int main() {
    {
        aoe::impl::StaticApiKey api_key("123");
        aoe::impl::StaticSecretKey secret_key("456");
        aoe::hmac_sha256::impl::Signer signer(api_key, secret_key);
    }
    return 0;
}