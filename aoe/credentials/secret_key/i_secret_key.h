#pragma once
#include <string_view>

namespace aoe {
class SecretKeyInterface {
  public:
    virtual std::string_view SecretKey() = 0;
    virtual ~SecretKeyInterface()        = default;
};
};  // namespace aoe