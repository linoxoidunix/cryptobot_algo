#pragma once
#include <string_view>

namespace aoe {
namespace ed25519 {
class SecretKeyBase64URLSafetyNoPaddingInterface {
  public:
    virtual std::string_view SecretKey() = 0;
    virtual std::pair<bool, const unsigned char*> GetSecretKey64Bytes()
        const                                             = 0;
    virtual ~SecretKeyBase64URLSafetyNoPaddingInterface() = default;
};

class SecretKeyByte64Interface {
  public:
    virtual std::pair<bool, const unsigned char*> GetSecretKey64Bytes()
        const                           = 0;
    virtual ~SecretKeyByte64Interface() = default;
};
};  // namespace ed25519
};  // namespace aoe