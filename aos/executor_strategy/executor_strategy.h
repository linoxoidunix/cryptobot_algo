#pragma once
#include "aos/executor_strategy/i_executor_strategy.h"

namespace aos {
namespace impl {
template <typename HashTAsset, typename Price>
class ExecutorStrategy : public ExecutorStrategyInterface<HashTAsset, Price> {
  public:
    ~ExecutorStrategy() override = default;
    void Run(StrategyInterface<HashTAsset, Price>& strategy) override {

    };
};
};  // namespace impl
};  // namespace aos
