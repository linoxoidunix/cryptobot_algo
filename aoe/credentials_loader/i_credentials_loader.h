#pragma once

namespace aoe {
class Ed25519CredentialsLoaderInterface {
  public:
    virtual ~Ed25519CredentialsLoaderInterface()                     = default;

    virtual std::pair<bool, std::string_view> ApiKeyEd25519MainNet() = 0;
    virtual std::pair<bool, std::string_view>
    SecretKeyEd25519MainNetBase64URLSafetyNoPadding()                = 0;
    virtual std::pair<bool, std::string_view> ApiKeyEd25519TestNet() = 0;
    virtual std::pair<bool, std::string_view>
    SecretKeyEd25519TestNetBase64URLSafetyNoPadding() = 0;
};

class HmacSha256CredentialsLoaderInterface {
  public:
    virtual ~HmacSha256CredentialsLoaderInterface() = default;

    virtual std::pair<bool, std::string_view> ApiKeyHmacSha256MainNet()    = 0;
    virtual std::pair<bool, std::string_view> SecretKeyHmacSha256MainNet() = 0;
    virtual std::pair<bool, std::string_view> ApiKeyHmacSha256TestNet()    = 0;
    virtual std::pair<bool, std::string_view> SecretKeyHmacSha256TestNet() = 0;
};

class CredentialsLoaderInterface : public Ed25519CredentialsLoaderInterface,
                                   public HmacSha256CredentialsLoaderInterface {
  public:
    virtual ~CredentialsLoaderInterface() = default;
};
};  // namespace aoe