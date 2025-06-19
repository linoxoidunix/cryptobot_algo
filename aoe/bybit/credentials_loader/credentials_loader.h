#pragma once
#include <memory>
#include <string_view>

#include "aoe/credentials/api_key/i_api_key.h"
#include "aoe/credentials/secret_key/i_secret_key.h"
#include "aoe/credentials_loader/i_credentials_loader.h"
#define TOML_EXCEPTIONS 0
#include "toml++/toml.hpp"

namespace aoe {
namespace bybit {
namespace impl {
class CredentialsLoader : public CredentialsLoaderInterface {
    toml::table config_;
    bool config_loaded_ = false;

  public:
    explicit CredentialsLoader(std::string& config_path) {
        toml::parse_result result = toml::parse_file(config_path);
        if (result) {
            config_loaded_ = true;
            config_        = result.table();
        }
    };
    std::pair<bool, std::string_view> ApiKeyHmacSha256MainNet() override {
        return Get("bybit", "hmac_sha256", "mainnet", "api_key");
    };
    std::pair<bool, std::string_view> SecretKeyHmacSha256MainNet() override {
        return Get("bybit", "hmac_sha256", "mainnet", "secret_key");
    };
    std::pair<bool, std::string_view> ApiKeyHmacSha256TestNet() override {
        return Get("bybit", "hmac_sha256", "testnet", "api_key");
    };
    std::pair<bool, std::string_view> SecretKeyHmacSha256TestNet() override {
        return Get("bybit", "hmac_sha256", "testnet", "secret_key");
    };
    std::pair<bool, std::string_view> ApiKeyEd25519MainNet() override {
        return {false, {}};
    };
    std::pair<bool, std::string_view>
    SecretKeyEd25519MainNetBase64URLSafetyNoPadding() override {
        return {false, {}};
    };
    std::pair<bool, std::string_view> ApiKeyEd25519TestNet() override {
        return {false, {}};
    };
    std::pair<bool, std::string_view>
    SecretKeyEd25519TestNetBase64URLSafetyNoPadding() override {
        return {false, {}};
    };

  private:
    std::pair<bool, std::string_view> Get(std::string_view group,
                                          std::string_view type_key,
                                          std::string_view network,
                                          std::string_view key) const {
        if (!config_loaded_) return {false, {}};

        const auto* table = config_[group][type_key][network].as_table();
        if (!table) return {false, {}};

        const auto* value = table->get_as<std::string>(key);
        if (!value) return {false, {}};

        return {true, std::string_view(value->get())};
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe