#pragma once

#include "aoe/binance/order_manager/i_order_manager.h"
#include "aoe/binance/order_watcher/i_order_watcher.h"
#include "fmtlog.h"

namespace aoe {
namespace binance {
namespace impl {
template <template <typename> typename MemoryPool>
class OrderWatcher : public OrderWatcherInterface<MemoryPool> {
    OrderManagerInterface<MemoryPool>& order_manager_;

  public:
    OrderWatcher(OrderManagerInterface<MemoryPool>& order_manager)
        : order_manager_(order_manager) {};
    void OnEvent(boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) {
        order_manager_.OnResponse(event);
    };
    ~OrderWatcher() = default;
};
};  // namespace impl
};  // namespace binance
};  // namespace aoe