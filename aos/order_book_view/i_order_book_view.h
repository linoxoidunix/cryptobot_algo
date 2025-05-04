#pragma once
#include <vector>

#include "aos/order_book_level_raw/order_book_level_raw.h"
namespace aos {
template <typename Price, typename Qty>
class OrderBookTwoSideViewInterface {
  public:
    virtual std::vector<OrderBookLevelRaw<Price, Qty>>& Bids() = 0;
    virtual std::vector<OrderBookLevelRaw<Price, Qty>>& Asks() = 0;
};
};  // namespace aos
