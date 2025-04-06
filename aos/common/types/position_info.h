#pragma once

namespace aos {
template <typename Price, typename Qty>
struct PositionInfo {
    Price avg_price{0};
    Qty net_qty{0};

    bool operator==(const PositionInfo& other) const {
        return avg_price == other.avg_price && net_qty == other.net_qty;
    }
};
}  // namespace aos
