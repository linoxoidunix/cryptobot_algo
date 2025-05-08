#pragma once
#include "aos/common/ref_counted.h"
#include "aos/order_book_view/i_order_book_view.h"
namespace aos {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookEventInterface
    : public common::RefCounted<
          MemoryPool, OrderBookEventInterface<Price, Qty, MemoryPool>>,
      public OrderBookTwoSideViewInterface<Price, Qty> {
  public:
    virtual ~OrderBookEventInterface() = default;
};
};  // namespace aos