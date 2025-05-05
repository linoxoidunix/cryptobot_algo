#pragma once
#include "aoe/bybit/order_book_event/i_order_book_event.h"
#include "boost/intrusive_ptr.hpp"
namespace aoe {
namespace bybit {

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookSyncInterface {
  public:
    virtual ~OrderBookSyncInterface() = default;
    virtual void OnEvent(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) = 0;
};
};  // namespace bybit
};  // namespace aoe
