#pragma once
// #include "aoe/binance/order_book_event/i_order_book_event.h"
#include "boost/intrusive_ptr.hpp"

namespace aoe {
namespace binance {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookEventInterface;
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookSnapshotEventInterface;
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookDiffEventInterface;
}  // namespace binance
}  // namespace aoe

namespace aoe {
namespace binance {

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookSyncInterface {
  public:
    virtual ~OrderBookSyncInterface() = default;
    virtual void OnEvent(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) = 0;
    virtual void AcceptSnapshot(
        boost::intrusive_ptr<
            OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
            ptr)                     = 0;
    virtual void AcceptDiff(boost::intrusive_ptr<
                            OrderBookDiffEventInterface<Price, Qty, MemoryPool>>
                                ptr) = 0;
};
};  // namespace binance
};  // namespace aoe
