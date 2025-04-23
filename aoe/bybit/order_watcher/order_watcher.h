#pragma once

#include "aoe/bybit/order_storage/i_order_storage.h"
#include "aoe/bybit/order_watcher/i_order_watcher.h"
#include "aot/Logger.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class OrderWatcher : OrderWatcherInterface<MemoryPool> {
    OrderStorageInterface& order_storage_;

  public:
    OrderWatcher(OrderStorageInterface& order_storage)
        : order_storage_(order_storage) {};
    void OnOrderEvent(
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) {
        event->Accept(order_storage_);
    };
    ~OrderWatcher() = default;
};
};  // namespace bybit
};  // namespace aoe