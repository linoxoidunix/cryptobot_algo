#pragma once
#include <boost/intrusive_ptr.hpp>

#include "aoe/binance/order_event/i_order_event.h"
namespace aoe {
namespace binance {
template <template <typename> typename MemoryPool>
class OrderWatcherInterface {
  public:
    virtual void OnEvent(
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) = 0;
    ~OrderWatcherInterface()                                         = default;
};
};  // namespace binance
};  // namespace aoe