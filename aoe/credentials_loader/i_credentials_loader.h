#pragma once

namespace aoe {
class CredentialsLoaderInterface {
  public:
    virtual ~CredentialsLoaderInterface()                        = default;
    virtual std::pair<bool, std::string_view> ApiKeyMainNet()    = 0;
    virtual std::pair<bool, std::string_view> SecretKeyMainNet() = 0;
    virtual std::pair<bool, std::string_view> ApiKeyTestNet()    = 0;
    virtual std::pair<bool, std::string_view> SecretKeyTestNet() = 0;
};
};  // namespace aoe