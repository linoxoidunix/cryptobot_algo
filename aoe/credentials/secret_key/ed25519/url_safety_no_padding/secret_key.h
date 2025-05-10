#pragma once
#include <cstring>  // для std::memcpy
#include <string>

#include "aoe/credentials/secret_key/ed25519/url_safety_no_padding/i_secret_key.h"
#include "sodium.h"
namespace aoe {
namespace ed25519 {
namespace impl {

class StaticSecretKeyBase64URLSafetyNoPadding
    : public SecretKeyBase64URLSafetyNoPaddingInterface {
    int rc_ = -1;
    unsigned char sk_[crypto_sign_SECRETKEYBYTES];
    std::string secret_key_;
    bool returned_status = false;

  public:
    StaticSecretKeyBase64URLSafetyNoPadding(std::string_view secret_key)
        : secret_key_(secret_key.data()) {
        size_t sk_len;

        rc_ = sodium_base642bin(sk_, sizeof(sk_), secret_key_.data(),
                                secret_key_.size(), nullptr, &sk_len, nullptr,
                                sodium_base64_VARIANT_URLSAFE_NO_PADDING);
        if (rc_ != 0) {
            returned_status = false;
            return;
        }
        if (sk_len != crypto_sign_SECRETKEYBYTES) {
            returned_status = false;
            return;
        }
        returned_status = true;
    }
    std::string_view SecretKey() override { return secret_key_; }
    /**
     * @brief Returns the Ed25519 secret key as a 64-byte array.
     *
     * The secret key is composed of:
     * - 32 bytes of the private key (seed/private scalar),
     * - followed by 32 bytes of the corresponding public key.
     *
     * This format is required by libsodium's `crypto_sign_detached` function.
     *
     * @return std::pair<bool, const unsigned char*>
     * - `first`: true if the key was successfully decoded and is ready for use.
     * - `second`: pointer to a buffer of length `crypto_sign_SECRETKEYBYTES`
     * (64 bytes) containing the full Ed25519 secret key.
     */
    std::pair<bool, const unsigned char*> GetSecretKey64Bytes() const override {
        return std::make_pair(returned_status, sk_);
    }
    ~StaticSecretKeyBase64URLSafetyNoPadding() override = default;
};

class StaticSecretKeyBase64 : public SecretKeyByte64Interface {
    unsigned char sk_[crypto_sign_SECRETKEYBYTES];

  public:
    StaticSecretKeyBase64(unsigned char (&sk)[crypto_sign_SECRETKEYBYTES]) {
        std::memcpy(sk_, sk, crypto_sign_SECRETKEYBYTES);
    }
    std::pair<bool, const unsigned char*> GetSecretKey64Bytes() const override {
        return std::make_pair(true, sk_);
    }
    ~StaticSecretKeyBase64() override = default;
};
}  // namespace impl
};  // namespace ed25519
};  // namespace aoe