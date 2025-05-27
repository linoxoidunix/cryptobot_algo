#pragma once

namespace aos {
template <typename Price, typename Qty>
struct BestBid {
    Price bid_price;
    Qty bid_qty;
};
};  // namespace aos