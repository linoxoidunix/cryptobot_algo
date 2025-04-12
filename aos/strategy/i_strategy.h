#pragma once
#include <functional>
#include <queue>

namespace aos {
template <typename HashT, typename T>
class AddActionsToBuyInteface {
  public:
    virtual void AddActionsToBuy(
        std::function<void(std::queue<HashT>& queue_to_buy, const HashT hasht,
                           const T& value)>
            observer) = 0;
};

template <typename HashT, typename T>
class AddActionsToSellInteface {
  public:
    virtual void AddActionsToSell(
        std::function<void(std::queue<HashT>& queue_to_buy, const HashT hasht,
                           const T& value)>
            observer) = 0;
};

template <typename HashT, typename T>
class StrategyInterface : public AddActionsToBuyInteface<HashT, T>,
                          public AddActionsToSellInteface<HashT, T> {
  public:
    virtual void AnalyzeToBuy(const HashT asset, const T& value)  = 0;
    virtual void AnalyzeToSell(const HashT asset, const T& value) = 0;
};
};  // namespace aos