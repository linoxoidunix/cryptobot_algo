#pragma once
#include "aos/order_book_view/i_order_book_view.h"
namespace aos {
template <typename Price, typename Qty>
class OrdeBookSnapshotInterface
    : public OrderBookTwoSideViewInterface<Price, Qty> {
  public:
    virtual ~OrdeBookSnapshotInterface() = default;
};
};  // namespace aos