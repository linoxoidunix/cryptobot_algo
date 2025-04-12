#pragma once
namespace aos {
namespace impl {
template <typename Price, typename Qty>
class RealizedPnlCalculatorInterface {
  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());

    virtual RealizedPnl Calculate(Price avg_price, Qty net_qty, Price price,
                                  Qty qty)    = 0;
    virtual ~RealizedPnlCalculatorInterface() = default;
};
};  // namespace impl
};  // namespace aos