#pragma once
namespace aos {
namespace impl {
template <typename Price, typename Qty>
class RealizedPnlCalculator {
  public:
    static Price Calculate(Price avg_price, Qty net_qty, Price price, Qty qty) {
        Qty abs_qty = std::abs(qty);
        Qty min_qty = std::min(std::abs(net_qty), abs_qty);
        min_qty     = (qty < 0) ? -min_qty : min_qty;
        return min_qty * (price - avg_price);
    }
};
};  // namespace impl
};  // namespace aos