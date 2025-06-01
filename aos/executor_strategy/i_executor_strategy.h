#pragma once
#include "aos/strategy/i_strategy.h"

namespace aos {
template <typename HashTAsset, typename Price>
class ExecutorStrategyInterface {
  public:
    virtual ~ExecutorStrategyInterface()                             = default;
    virtual void Run(StrategyInterface<HashTAsset, Price>& strategy) = 0;
};
};  // namespace aos
