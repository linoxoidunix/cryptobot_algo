#pragma once

#include "aoe/bybit/order_manager/i_order_manager.h"
#include "aoe/bybit/order_watcher/i_order_watcher.h"
#include "aos/logger/mylog.h"

namespace aoe {
namespace bybit {
namespace impl {
template <template <typename> typename MemoryPool>
class OrderWatcher : public OrderWatcherInterface<MemoryPool> {
    OrderManagerInterface<MemoryPool>& order_manager_;

  public:
    explicit OrderWatcher(OrderManagerInterface<MemoryPool>& order_manager)
        : order_manager_(order_manager) {};
    void OnEvent(
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) override {
        order_manager_.OnResponse(event);
    };
    ~OrderWatcher() override = default;
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe