#pragma once
#include <cassert>
#include <cstdint>

#include "aoe/credentials/api_key/i_api_key.h"
#include "aoe/credentials/secret_key/ed25519/url_safety_no_padding/i_secret_key.h"
#include "aoe/signer/ed25519/i_signer.h"
#include "aot/Logger.h"
#include "boost/algorithm/string.hpp"
#include "sodium.h"
namespace aoe {
namespace ed25519 {
namespace impl {

/**
 * @brief Ed25519-based signer implementation.
 *
 * This class expects the secret key to be provided in base64 URL-safe
 * no-padding format (i.e., encoded with
 * sodium_base64_VARIANT_URLSAFE_NO_PADDING).
 */
class SignerBase64URLKey : public SignerInterface {
    ApiKeyInterface& api_key_;
    SecretKeyBase64URLSafetyNoPaddingInterface& secret_key_;

    inline std::string Base64EncodeUrlSafeNoPadding(const unsigned char* data,
                                                    size_t len) {
        const size_t max_encoded_len = sodium_base64_encoded_len(
            len, sodium_base64_VARIANT_URLSAFE_NO_PADDING);
        std::string encoded(max_encoded_len, '\0');
        sodium_bin2base64(encoded.data(), encoded.size(), data, len,
                          sodium_base64_VARIANT_URLSAFE_NO_PADDING);
        encoded.resize(strlen(encoded.c_str()));  // Убираем лишние нули
        return encoded;
    }
    inline std::string Base64EncodeStandard(const unsigned char* data,
                                            size_t len) {
        const size_t max_encoded_len =
            sodium_base64_encoded_len(len, sodium_base64_VARIANT_ORIGINAL);
        std::string encoded(max_encoded_len, '\0');
        sodium_bin2base64(encoded.data(), encoded.size(), data, len,
                          sodium_base64_VARIANT_ORIGINAL);
        encoded.resize(strlen(encoded.c_str()));  // Убираем лишние нули
        return encoded;
    }

  public:
    SignerBase64URLKey(ApiKeyInterface& api_key,
                       SecretKeyBase64URLSafetyNoPaddingInterface& secret_key)
        : api_key_(api_key), secret_key_(secret_key) {}
    /**
     * @brief Signs the input message using Ed25519 and returns the
     * signature encoded as base64 URL-safe with no padding.
     *
     * @param data The message to sign.
     * @return std::pair<bool, std::string>
     *         - First: success flag.
     *         - Second: base64 URL-safe no-padding encoded signature
     * string.
     *
     * @note The secret key must be provided in base64 URL-safe no-padding
     * format.
     */
    std::pair<bool, std::string> SignBase64EncodeUrlSafeNoPadding(
        std::string_view data) override {
        unsigned char signature[crypto_sign_BYTES];

        auto [status, sk] = secret_key_.GetSecretKey64Bytes();
        if (!status) {
            return {false, {}};
        }
        // Подпись
        crypto_sign_detached(
            signature, nullptr,
            reinterpret_cast<const unsigned char*>(data.data()), data.size(),
            sk);

        return {true,
                Base64EncodeUrlSafeNoPadding(signature, crypto_sign_BYTES)};
    }
    /**
     * @brief Signs the input message using Ed25519 and returns the signature
     * encoded in standard base64 (with padding).
     *
     * @param data The message to sign.
     * @return std::pair<bool, std::string>
     *         - First: success flag.
     *         - Second: base64 (padded) encoded signature string.
     *
     * @note The secret key must be provided in base64 URL-safe no-padding
     * format.
     */
    std::pair<bool, std::string> SignBase64EncodeStandard(
        std::string_view data) override {
        unsigned char signature[crypto_sign_BYTES];

        // base64URLSAFE_NO_PADDING
        auto sk_base64_urlsafe_no_padding = secret_key_.SecretKey();
        unsigned char sk[crypto_sign_SECRETKEYBYTES];
        size_t sk_len;
        int rc;
        rc = sodium_base642bin(
            sk, sizeof(sk), sk_base64_urlsafe_no_padding.data(),
            sk_base64_urlsafe_no_padding.size(), nullptr, &sk_len, nullptr,
            sodium_base64_VARIANT_URLSAFE_NO_PADDING);
        if (rc != 0) return {false, {}};
        if (sk_len != crypto_sign_SECRETKEYBYTES) return {false, {}};
        // Подпись
        crypto_sign_detached(
            signature, nullptr,
            reinterpret_cast<const unsigned char*>(data.data()), data.size(),
            sk);

        return {true, Base64EncodeStandard(signature, crypto_sign_BYTES)};
    }

    std::string_view ApiKey() override { return api_key_.ApiKey(); }

    ~SignerBase64URLKey() override = default;
};

/**
 * @brief Ed25519-based signer implementation.
 *
 * This class expects the secret key to be provided in base64 URL-safe
 * no-padding format (i.e., encoded with
 * sodium_base64_VARIANT_URLSAFE_NO_PADDING).
 */
class SignerRaw64ByteKey : public SignerInterface {
    ApiKeyInterface& api_key_;
    SecretKeyByte64Interface& secret_key_;

    inline std::string Base64EncodeUrlSafeNoPadding(const unsigned char* data,
                                                    size_t len) {
        const size_t max_encoded_len = sodium_base64_encoded_len(
            len, sodium_base64_VARIANT_URLSAFE_NO_PADDING);
        std::string encoded(max_encoded_len, '\0');
        sodium_bin2base64(encoded.data(), encoded.size(), data, len,
                          sodium_base64_VARIANT_URLSAFE_NO_PADDING);
        encoded.resize(strlen(encoded.c_str()));  // Убираем лишние нули
        return encoded;
    }
    inline std::string Base64EncodeStandard(const unsigned char* data,
                                            size_t len) {
        const size_t max_encoded_len =
            sodium_base64_encoded_len(len, sodium_base64_VARIANT_ORIGINAL);
        std::string encoded(max_encoded_len, '\0');
        sodium_bin2base64(encoded.data(), encoded.size(), data, len,
                          sodium_base64_VARIANT_ORIGINAL);
        encoded.resize(strlen(encoded.c_str()));  // Убираем лишние нули
        return encoded;
    }

  public:
    SignerRaw64ByteKey(ApiKeyInterface& api_key,
                       SecretKeyByte64Interface& secret_key)
        : api_key_(api_key), secret_key_(secret_key) {}
    /**
     * @brief Signs the input message using Ed25519 and returns the
     * signature encoded as base64 URL-safe with no padding.
     *
     * @param data The message to sign.
     * @return std::pair<bool, std::string>
     *         - First: success flag.
     *         - Second: base64 URL-safe no-padding encoded signature
     * string.
     *
     * @note The secret key must be provided in base64 URL-safe no-padding
     * format.
     */
    std::pair<bool, std::string> SignBase64EncodeUrlSafeNoPadding(
        std::string_view data) override {
        unsigned char signature[crypto_sign_BYTES];

        auto [status, sk] = secret_key_.GetSecretKey64Bytes();
        if (!status) {
            return {false, {}};
        }
        // Подпись
        crypto_sign_detached(
            signature, nullptr,
            reinterpret_cast<const unsigned char*>(data.data()), data.size(),
            sk);

        return {true,
                Base64EncodeUrlSafeNoPadding(signature, crypto_sign_BYTES)};
    }
    /**
     * @brief Signs the input message using Ed25519 and returns the signature
     * encoded in standard base64 (with padding).
     *
     * @param data The message to sign.
     * @return std::pair<bool, std::string>
     *         - First: success flag.
     *         - Second: base64 (padded) encoded signature string.
     *
     * @note The secret key must be provided in base64 URL-safe no-padding
     * format.
     */
    std::pair<bool, std::string> SignBase64EncodeStandard(
        std::string_view data) override {
        unsigned char signature[crypto_sign_BYTES];

        auto [status, sk] = secret_key_.GetSecretKey64Bytes();
        if (!status) {
            return {false, {}};
        }
        // Подпись
        crypto_sign_detached(
            signature, nullptr,
            reinterpret_cast<const unsigned char*>(data.data()), data.size(),
            sk);

        return {true, Base64EncodeStandard(signature, crypto_sign_BYTES)};
    }

    std::string_view ApiKey() override { return api_key_.ApiKey(); }

    ~SignerRaw64ByteKey() override = default;
};

};  // namespace impl
};  // namespace ed25519
};  // namespace aoe