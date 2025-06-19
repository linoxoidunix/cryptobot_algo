#pragma once
#include "aoe/credentials/api_key/i_api_key.h"
namespace aoe {
namespace impl {
class StaticApiKey : public ApiKeyInterface {
    std::string api_key_;

  public:
    explicit StaticApiKey(std::string_view api_key)
        : api_key_(api_key.data()) {}
    std::string_view ApiKey() override { return api_key_; }
    ~StaticApiKey() override = default;
};
};  // namespace impl
};  // namespace aoe