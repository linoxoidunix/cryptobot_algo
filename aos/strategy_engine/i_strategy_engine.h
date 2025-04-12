#pragma once

namespace aos {
template <typename HashT, typename T>
class StrategyEngineInterface {
  public:
    virtual void AddData(const HashT has_asset, const T& value) = 0;
    virtual ~StrategyEngineInterface()                          = default;
};
};  // namespace aos