#pragma once
#include <memory>
#include <string_view>

#include "aoe/credentials/api_key/i_api_key.h"
#include "aoe/credentials/secret_key/i_secret_key.h"
#include "aoe/credentials_loader/i_credentials_loader.h"
#include "toml++/toml.hpp"

namespace aoe {
namespace bybit {
namespace impl {
class CredentialsLoader : public CredentialsLoaderInterface {
    toml::table config_;
    bool config_loaded_ = false;

  public:
    CredentialsLoader(std::string& config_path) {
        try {
            config_        = toml::parse_file(config_path);
            config_loaded_ = true;
        } catch (const std::exception& e) {
        }
    };
    std::pair<bool, std::string_view> ApiKeyMainNet() override {
        return Get("bybit", "mainnet", "api_key");
    };
    std::pair<bool, std::string_view> SecretKeyMainNet() override {
        return Get("bybit", "mainnet", "secret_key");
    };
    std::pair<bool, std::string_view> ApiKeyTestNet() override {
        return Get("bybit", "testnet", "api_key");
    };
    std::pair<bool, std::string_view> SecretKeyTestNet() override {
        return Get("bybit", "testnet", "secret_key");
    };

  private:
    std::pair<bool, std::string_view> Get(const std::string& group,
                                          const std::string& network,
                                          const std::string& key) const {
        if (!config_loaded_) return {false, {}};

        auto* table = config_[group][network].as_table();
        if (!table) return {false, {}};

        const auto* value = table->get_as<std::string>(key);
        if (!value) return {false, {}};

        return {true, std::string_view(value->get())};
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe