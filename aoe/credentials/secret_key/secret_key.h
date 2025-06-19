#pragma once
#include "aoe/credentials/secret_key/i_secret_key.h"
namespace aoe {
namespace impl {
class StaticSecretKey : public SecretKeyInterface {
    std::string secret_key_;

  public:
    explicit StaticSecretKey(std::string_view secret_key)
        : secret_key_(secret_key.data()) {}
    std::string_view SecretKey() override { return secret_key_; }
    ~StaticSecretKey() override = default;
};
};  // namespace impl
};  // namespace aoe