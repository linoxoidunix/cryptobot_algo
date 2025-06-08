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
    virtual ~AddActionsToBuyInteface() = default;
};

template <typename HashT, typename T>
class AddActionsToSellInteface {
  public:
    virtual void AddActionsToSell(
        std::function<void(std::queue<HashT>& queue_to_buy, const HashT hasht,
                           const T& value)>
            observer) = 0;
    virtual ~AddActionsToSellInteface() = default;
};

template <typename HashT, typename T>
class TradeAnalyserInterface {
  public:
    virtual void AnalyzeToBuy(const HashT asset, const T& value)  = 0;
    virtual void AnalyzeToSell(const HashT asset, const T& value)  = 0;
    virtual ~TradeAnalyserInterface() = default;
};

template <typename HashT, typename T>
class CoreStrategyInterface : public AddActionsToBuyInteface<HashT, T>,
                          public AddActionsToSellInteface<HashT, T>,
                          TradeAnalyserInterface<HashT, T> {
  public:
    virtual ~CoreStrategyInterface() = default;
};

template <typename HashT, typename T>
class StrategyInterface : public TradeAnalyserInterface<HashT, T> {
  public:
    virtual void AddData(const HashT has_asset, const T& value) = 0;
    virtual ~StrategyInterface() = default;
};
};  // namespace aos