#pragma once
#include <string_view>
namespace aoe {
class ApiKeyInterface {
  public:
    virtual std::string_view ApiKey() = 0;
    virtual ~ApiKeyInterface()        = default;
};
};  // namespace aoe