#pragma once
#include <string>
#include <string_view>

#include "aoe/credentials/api_key/i_api_key.h"
namespace aoe {
namespace ed25519 {
class SignerInterface : public ApiKeyInterface {
  public:
    virtual std::pair<bool, std::string> SignBase64EncodeUrlSafeNoPadding(
        std::string_view data) = 0;
    virtual std::pair<bool, std::string> SignBase64EncodeStandard(
        std::string_view data)  = 0;

    ~SignerInterface() override = default;
};
};  // namespace ed25519
};  // namespace aoe