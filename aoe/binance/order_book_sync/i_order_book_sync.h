#pragma once
#include "boost/intrusive_ptr.hpp"

namespace aoe {
namespace binance {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookEventInterface;
};
};  // namespace aoe
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
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) = 0;
    virtual void AcceptDiff(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) = 0;
};
};  // namespace binance
};  // namespace aoe
