#pragma once
#include "aos/order_book/order_book.h"
#include "aot/Logger.h"
namespace aoe {
namespace bybit {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename HashMap>
class OrderBookSync : public OrderBookSyncInterface<Price, Qty, MemoryPool> {
    aos::OrderBookEventListener<Price, Qty, MemoryPool, HashMap>& order_book_;

  public:
    OrderBookSync(aos::OrderBookEventListener<Price, Qty, MemoryPool, HashMap>&
                      order_book)
        : order_book_(order_book) {}
    ~OrderBookSync() override = default;
    void OnEvent(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) override {
        logi("listen message");
    };
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe