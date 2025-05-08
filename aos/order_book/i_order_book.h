#pragma once
#include <utility>  // std::pair

#include "aos/bbo_full/bbo_full.h"
#include "aos/order_book_event/i_order_book_event.h"
#include "boost/intrusive_ptr.hpp"

namespace aos {
template <typename Price, typename Qty>
class OrderBookInnerInterface {
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
    virtual ~OrderBookInnerInterface()             = default;
};

template <typename Price, typename Qty>
class HasBBOInterface {
  public:
    virtual std::pair<bool, BBOFull<Price, Qty>> GetBBO() = 0;
    virtual ~HasBBOInterface()                            = default;
};

class ClearOrderBookInterface {
  public:
    virtual void Clear()               = 0;
    virtual ~ClearOrderBookInterface() = default;
};

template <typename Price, typename Qty>
class OrderBookInterface : public ClearOrderBookInterface {
  public:
    // add bid level
    virtual void AddBidOrder(Price price, Qty qty) = 0;
    // remove bid lvl
    virtual void AddAskOrder(Price price, Qty qty) = 0;
    // remove ask lvl
    virtual ~OrderBookInterface()                  = default;
};

template <typename Price, typename Qty, template <typename> class MemoryPool>
class OrderBookEventListenerInterface : public ClearOrderBookInterface {
  public:
    // add bid level
    virtual void OnEvent(boost::intrusive_ptr<
                         OrderBookEventInterface<Price, Qty, MemoryPool>>) = 0;
    virtual ~OrderBookEventListenerInterface() = default;
};
};  // namespace aos