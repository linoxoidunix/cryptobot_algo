#pragma once

namespace aos {
class OrderBookSubscriberInterface {
  public:
    virtual void OnOrderBookUpdate()        = 0;
    virtual ~OrderBookSubscriberInterface() = default;
};
};  // namespace aos