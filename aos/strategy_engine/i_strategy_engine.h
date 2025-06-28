#pragma once
#include "aos/strategy/i_strategy.h"
namespace aos {

template <typename HashT, typename T>
class StrategyEngineInterface : public SetPriceWhenEnterInterface<HashT, T> {
  public:
    virtual void AddData(const HashT& has_asset, const T& value) = 0;
    virtual ~StrategyEngineInterface()                           = default;
};
};  // namespace aos