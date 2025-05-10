#include <gtest/gtest.h>
#include <sodium.h>

#include "aoe/aoe.h"
#include "aoe/credentials/secret_key/ed25519/url_safety_no_padding/secret_key.h"
#include "aoe/signer/ed25519/signer.h"

// Заглушка для ApiKeyInterface
class MockApiKey : public aoe::ApiKeyInterface {
  public:
    std::string_view ApiKey() override { return "test_api_key"; }
};

// Заглушка для SecretKeyInterface
class MockSecretKey : public aoe::SecretKeyInterface {
    std::string key_;

  public:
    MockSecretKey(const std::string& key) : key_(key) {}

    std::string_view SecretKey() override { return key_; }
};

TEST(SignerTest, Sign) {
    ASSERT_EQ(sodium_init(), 0);  // libsodium должен быть инициализирован

    // Сгенерировать ключи
    unsigned char pk[crypto_sign_PUBLICKEYBYTES];
    unsigned char sk[crypto_sign_SECRETKEYBYTES];
    crypto_sign_keypair(pk, sk);

    // Подготовка заглушек
    MockApiKey mock_api_key;
    aoe::ed25519::impl::StaticSecretKeyBase64 mock_secret_key(sk);

    aoe::ed25519::impl::SignerRaw64ByteKey signer(mock_api_key,
                                                  mock_secret_key);

    std::string_view message = "hello world";

    auto [ok, signature_b64] = signer.SignBase64EncodeUrlSafeNoPadding(message);

    ASSERT_TRUE(ok);
    ASSERT_FALSE(signature_b64.empty());

    // Декодировать base64 обратно
    unsigned char decoded[crypto_sign_BYTES];
    size_t decoded_len;
    int rc =
        sodium_base642bin(decoded, sizeof(decoded), signature_b64.c_str(),
                          signature_b64.size(), nullptr, &decoded_len, nullptr,
                          sodium_base64_VARIANT_URLSAFE_NO_PADDING);
    ASSERT_EQ(rc, 0);
    ASSERT_EQ(decoded_len, crypto_sign_BYTES);

    // Проверка валидности подписи
    int verify = crypto_sign_verify_detached(
        decoded, reinterpret_cast<const unsigned char*>(message.data()),
        message.size(), pk);
    ASSERT_EQ(verify, 0);  // 0 = OK
}
