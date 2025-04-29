#pragma once
#include <boost/intrusive_ptr.hpp>

#include "aoe/bybit/order_event/i_order_event.h"
namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class OrderWatcherInterface {
  public:
    virtual void OnEvent(
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) = 0;
    ~OrderWatcherInterface()                                         = default;
};
};  // namespace bybit
};  // namespace aoe