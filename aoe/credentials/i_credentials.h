#pragma once
#include <string_view>

#include "aoe/credentials/api_key/i_api_key.h"
#include "aoe/credentials/secret_key/i_secret_key.h"

namespace aoe {
class CredentialsInterface : public SecretKeyInterface, public ApiKeyInterface {
  public:
    virtual ~CredentialsInterface() = default;
};
};  // namespace aoe