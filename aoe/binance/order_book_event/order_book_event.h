#pragma once
#include <vector>

#include "aoe/binance/order_book_event/i_order_book_event.h"
#include "aos/order_book_level_raw/order_book_level_raw.h"
namespace aoe {
namespace binance {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookSnapshotEventDefault
    : public OrderBookSnapshotEventInterface<Price, Qty, MemoryPool> {
  public:
    OrderBookSnapshotEventDefault() = default;
    ~OrderBookSnapshotEventDefault() override {};
    void Accept(spot::OrderBookSyncInterface<Price, Qty, MemoryPool>& sync) override {
        logi("process event as snapshot");
        sync.AcceptSnapshot(this);
    };
    void Accept(futures::OrderBookSyncInterface<Price, Qty, MemoryPool>& sync) override {
        logi("process event as snapshot");
        sync.AcceptSnapshot(this);
    };
};

// template <typename Price, typename Qty, template <typename> typename
// MemoryPool> class OrderBookDiffEventDefault
//     : public OrderBookDiffEventInterface<Price, Qty, MemoryPool> {
//   public:
//     OrderBookDiffEventDefault() = default;
//     ~OrderBookDiffEventDefault() override {};
//     void Accept(OrderBookSyncInterface<Price, Qty, MemoryPool>& sync)
//     override {
//         logi("process event as diff");
//         sync.AcceptDiff(this);
//     };
// };
namespace spot {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookDiffEventDefault
    : public aoe::binance::spot::OrderBookDiffEventInterface<Price, Qty,
                                                             MemoryPool> {
  public:
    OrderBookDiffEventDefault() = default;
    ~OrderBookDiffEventDefault() override {};
    void Accept(
        aoe::binance::spot::OrderBookSyncInterface<Price, Qty, MemoryPool>&
            sync) override {
        logi("process spot event as diff");
        sync.AcceptDiff(this);
    };
};
};  // namespace spot

namespace futures {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookDiffEventDefault
    : public aoe::binance::futures::OrderBookDiffEventInterface<Price, Qty,
                                                                MemoryPool> {
  public:
    OrderBookDiffEventDefault() = default;
    ~OrderBookDiffEventDefault() override {};
    void Accept(
        aoe::binance::futures::OrderBookSyncInterface<Price, Qty, MemoryPool>&
            sync) override {
        logi("process futures event as diff");
        sync.AcceptDiff(this);
    };
};
};  // namespace futures

};  // namespace impl

};  // namespace binance
};  // namespace aoe