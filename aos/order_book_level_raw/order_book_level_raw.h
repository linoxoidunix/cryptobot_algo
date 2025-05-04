#pragma once

namespace aos {
template <typename Price, typename Qty>
struct OrderBookLevelRaw {
    Price price;
    Qty qty;
};
};  // namespace aos