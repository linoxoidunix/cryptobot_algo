#pragma once
// #include "aoe/binance/order_book_event/i_order_book_event.h"
#include "boost/intrusive_ptr.hpp"

namespace aoe {
namespace binance {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookEventInterface;
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookSnapshotEventInterface;
namespace spot {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookDiffEventInterface;
};
namespace futures {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookDiffEventInterface;
};
};  // namespace binance
};  // namespace aoe

namespace aoe {
namespace binance {
namespace spot {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookSyncInterface {
  public:
    virtual ~OrderBookSyncInterface() = default;
    virtual void AcceptDiff(
        boost::intrusive_ptr<aoe::binance::spot::OrderBookDiffEventInterface<
            Price, Qty, MemoryPool>>
            ptr) = 0;
    virtual void AcceptSnapshot(
        boost::intrusive_ptr<
            OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
            ptr) = 0;
};
};  // namespace spot
namespace futures {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookSyncInterface {
  public:
    virtual ~OrderBookSyncInterface() = default;
    virtual void AcceptDiff(
        boost::intrusive_ptr<aoe::binance::futures::OrderBookDiffEventInterface<
            Price, Qty, MemoryPool>>
            ptr) = 0;
    virtual void AcceptSnapshot(
        boost::intrusive_ptr<
            OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
            ptr) = 0;
};
};  // namespace futures
};  // namespace binance
};  // namespace aoe
