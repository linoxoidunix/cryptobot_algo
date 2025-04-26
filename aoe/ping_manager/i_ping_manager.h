#pragma once

namespace aoe {
class PingManagerInterface {
  public:
    virtual ~PingManagerInterface() = default;
    virtual void Start()            = 0;
};

}  // namespace aoe