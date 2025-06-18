#pragma once

namespace aos {
template <typename Price, typename Qty>
struct BestAsk {
    Price ask_price;
    Qty ask_qty;
};
};  // namespace aos
