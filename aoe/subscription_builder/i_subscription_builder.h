#pragma once

namespace aoe {
class SubscriptionBuilderInterface {
  public:
    virtual ~SubscriptionBuilderInterface() = default;
    virtual void Subscribe()                = 0;
};
};  // namespace aoe