#pragma once

#include "aos/best_ask/best_ask.h"
#include "aos/best_bid/best_bid.h"

namespace aos {
class OrderBookSubscriberInterface {
  public:
    virtual void OnOrderBookUpdate()        = 0;
    virtual ~OrderBookSubscriberInterface() = default;
};

template <typename Price, typename Qty>
class OrderBookSubscriberCallbackSetterOnBestBidChangeInterface {
  public:
    virtual void SetCallback(
        std::function<void(BestBid<Price, Qty>& new_bid)> cb) = 0;
    virtual ~OrderBookSubscriberCallbackSetterOnBestBidChangeInterface() =
        default;
};

template <typename Price, typename Qty>
class OrderBookSubscriberCallbackSetterOnBestAskChangeInterface {
  public:
    virtual void SetCallback(
        std::function<void(BestAsk<Price, Qty>& new_ask)> cb) = 0;
    virtual ~OrderBookSubscriberCallbackSetterOnBestAskChangeInterface() =
        default;
};

template <typename Price, typename Qty>
class BestBidNotifierInterface
    : public OrderBookSubscriberInterface,
      public OrderBookSubscriberCallbackSetterOnBestBidChangeInterface<Price,
                                                                       Qty> {
  public:
    ~BestBidNotifierInterface() override = default;
};

template <typename Price, typename Qty>
class BestAskNotifierInterface
    : public OrderBookSubscriberInterface,
      public OrderBookSubscriberCallbackSetterOnBestAskChangeInterface<Price,
                                                                       Qty> {
  public:
    ~BestAskNotifierInterface() override = default;
};
};  // namespace aos