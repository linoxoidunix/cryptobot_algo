#pragma once
#include <vector>

#include "aoe/bybit/order_book_event/i_order_book_event.h"
#include "aos/order_book_level_raw/order_book_level_raw.h"
namespace aoe {
namespace bybit {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookSnapshotEventDefault
    : public OrderBookEventInterface<Price, Qty, MemoryPool> {
  public:
    OrderBookSnapshotEventDefault() = default;
    ~OrderBookSnapshotEventDefault() override {};
    void Accept(OrderBookSyncInterface<Price, Qty, MemoryPool>& sync) override {
        logi("process event as snapshot");
        sync.AcceptSnapshot(this);
    };
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookDiffEventDefault
    : public OrderBookEventInterface<Price, Qty, MemoryPool> {
  public:
    OrderBookDiffEventDefault() = default;
    ~OrderBookDiffEventDefault() override {};
    void Accept(OrderBookSyncInterface<Price, Qty, MemoryPool>& sync) override {
        logi("process event as diff");
        sync.AcceptDiff(this);
    };
};
};  // namespace impl

};  // namespace bybit
};  // namespace aoe