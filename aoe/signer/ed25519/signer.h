#pragma once
#include <cassert>
#include <cstdint>

#include "aoe/credentials/api_key/i_api_key.h"
#include "aoe/credentials/secret_key/i_secret_key.h"
#include "aoe/signer/ed25519/i_signer.h"
#include "boost/algorithm/string.hpp"
#include "sodium.h"
namespace aoe {
namespace ed25519 {
namespace impl {

class Signer : public SignerInterface {
    ApiKeyInterface& api_key_;
    SecretKeyInterface& secret_key_;

  public:
    Signer(ApiKeyInterface& api_key, SecretKeyInterface& secret_key)
        : api_key_(api_key), secret_key_(secret_key) {}

    std::string Sign(std::string_view data) override {
        unsigned char signature[crypto_sign_BYTES];

        const auto& sk = secret_key_.SecretKey();
        if (sk.size() != crypto_sign_SECRETKEYBYTES) {
            throw std::runtime_error("Invalid private key size");
        }

        crypto_sign_detached(
            signature,
            nullptr,  // signature length not needed for detached
            reinterpret_cast<const unsigned char*>(data.data()), data.size(),
            sk.data());

        return Base64Encode(signature, crypto_sign_BYTES);
    }

    std::string SignByLowerCase(std::string_view data) {
        std::string buffer = Sign(data);
        boost::algorithm::to_lower(buffer);
        return buffer;
    }

    std::string_view ApiKey() override { return api_key_.ApiKey(); }

    ~Signer() override = default;

  private:
    std::string Base64Encode(const unsigned char* data, size_t len) {
        char encoded[crypto_sign_BYTES * 2];  // more than enough
        sodium_bin2base64(encoded, sizeof(encoded), data, len,
                          sodium_base64_VARIANT_URLSAFE_NO_PADDING);
        return std::string(encoded);
    }
};
};  // namespace impl
};  // namespace ed25519
};  // namespace aoe