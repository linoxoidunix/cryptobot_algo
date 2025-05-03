#pragma once

namespace aos {
template <typename Price, typename Qty>
struct BBOFull {
    Price bid_price;
    Qty bid_qty;
    Price ask_price;
    Qty ask_qty;
};
};  // namespace aos