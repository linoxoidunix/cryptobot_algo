#pragma once

namespace aoe {
class AuthInterface {
  public:
    virtual void Auth()      = 0;
    virtual ~AuthInterface() = default;
};
};  // namespace aoe