#pragma once
#include <cassert>
#include <cstdint>

#include "aoe/credentials/api_key/i_api_key.h"
#include "aoe/credentials/secret_key/i_secret_key.h"
#include "aoe/signer/hmac_sha256/i_signer.h"
#include "boost/algorithm/string.hpp"
#include "openssl/hmac.h"
#include "openssl/sha.h"
namespace aoe {
namespace hmac_sha256 {
namespace impl {

class Signer : public SignerInterface {
    ApiKeyInterface& api_key_;
    SecretKeyInterface& secret_key_;

  public:
    Signer(ApiKeyInterface& api_key_, SecretKeyInterface& secret_key)
        : api_key_(api_key_), secret_key_(secret_key) {}
    std::string Sign(std::string_view data) override {
        std::uint8_t digest[EVP_MAX_MD_SIZE];
        std::uint32_t dilen{};

        auto p =
            ::HMAC(::EVP_sha256(), secret_key_.SecretKey().data(),
                   secret_key_.SecretKey().length(), (std::uint8_t*)data.data(),
                   data.size(), digest, &dilen);
        assert(p);

        return B2aHex(digest, dilen);
    }
    std::string SignByLowerCase(std::string_view data) {
        auto buffer = Sign(data);
        boost::algorithm::to_lower(buffer);
        return buffer;
    }
    std::string_view ApiKey() override { return api_key_.ApiKey(); }
    ~Signer() override = default;

  private:
    std::string B2aHex(const std::uint8_t* p, std::size_t n) {
        static const char hex[] = "0123456789abcdef";
        std::string res;
        res.reserve(n * 2);

        for (auto end = p + n; p != end; ++p) {
            const std::uint8_t v  = (*p);
            res                  += hex[(v >> 4) & 0x0F];
            res                  += hex[v & 0x0F];
        }

        return res;
    };
};
};  // namespace impl
};  // namespace hmac_sha256
};  // namespace aoe