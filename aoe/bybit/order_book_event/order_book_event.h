#pragma once
#include <vector>

#include "aoe/bybit/order_book_event/i_order_book_event.h"
#include "aos/order_book_level_raw/order_book_level_raw.h"
namespace aoe {
namespace bybit {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookSnapshotEventDefault
    : public OrderBookEventInterface<Price, Qty, MemoryPool> {
  public:
    OrderBookSnapshotEventDefault() = default;
    ~OrderBookSnapshotEventDefault() override {};
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookDiffEventDefault
    : public OrderBookEventInterface<Price, Qty, MemoryPool> {
  public:
    OrderBookDiffEventDefault() = default;
    ~OrderBookDiffEventDefault() override {};
};

};  // namespace bybit
};  // namespace aoe