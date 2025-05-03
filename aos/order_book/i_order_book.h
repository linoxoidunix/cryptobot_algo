#pragma once
#include <utility>  // std::pair

#include "aos/bbo_full/bbo_full.h"

namespace aos {
template <typename Price, typename Qty>
class OrderBookInterface {
  public:
    // add bid level
    virtual void AddBidLevel(Price price, Qty qty) = 0;
    // remove bid lvl
    virtual void RemoveBidLevel(Price price)       = 0;
    // add ask level
    virtual void AddAskLevel(Price price, Qty qty) = 0;
    // remove ask lvl
    virtual void RemoveAskLevel(Price price)       = 0;
    // clear order book
    virtual void Clear()                           = 0;
    virtual ~OrderBookInterface()                  = default;
};

template <typename Price, typename Qty>
class HasBBOInterface {
  public:
    virtual std::pair<bool, BBOFull<Price, Qty>> GetBBO() = 0;
    virtual ~HasBBOInterface()                            = default;
};
};  // namespace aos