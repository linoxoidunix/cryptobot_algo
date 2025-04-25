#pragma once

#include "aoe/bybit/order_manager/i_order_manager.h"
#include "aoe/bybit/order_watcher/i_order_watcher.h"
#include "aot/Logger.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class OrderWatcher : OrderWatcherInterface<MemoryPool> {
    OrderManagerInterface<MemoryPool>& order_manager_;

  public:
    OrderWatcher(OrderManagerInterface<MemoryPool>& order_manager)
        : order_manager_(order_manager) {};
    void OnOrderEvent(
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) {
        order_manager_.OnResponse(event);
    };
    ~OrderWatcher() = default;
};
};  // namespace bybit
};  // namespace aoe