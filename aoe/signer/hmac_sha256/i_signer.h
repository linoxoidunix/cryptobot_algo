#pragma once
#include <string>
#include <string_view>

#include "aoe/credentials/api_key/i_api_key.h"
namespace aoe {
namespace hmac_sha256 {
class SignerInterface : public ApiKeyInterface {
  public:
    virtual std::string Sign(std::string_view data)            = 0;
    virtual std::string SignByLowerCase(std::string_view data) = 0;

    ~SignerInterface() override                                = default;
};
};  // namespace hmac_sha256
};  // namespace aoe