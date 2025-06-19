#pragma once
#include <cmath>

#include "aoe/bybit/order_filter/i_order_filter.h"
#include "aos/order_min_qty_calculator/i_calculator.h"

namespace aoe {
namespace bybit {
namespace spot {
template <typename Price, typename Qty>
class OrderMinQtyCalculator
    : public aos::OrderMinQtyCalculatorInterface<Price, Qty> {
    OrderFilterInterface<Price, Qty>& filter_;

  private:
    static Qty RoundUpToStep(Price value, Price step) {
        return static_cast<Qty>(std::ceil(value / step) * step);
    }

    static bool IsAligned(Price value, Price step, Price eps = 1e-12) {
        Price mod = std::fmod(value, step);
        return (std::fabs(mod) <= eps) || (std::fabs(step - mod) <= eps);
    }

  public:
    explicit OrderMinQtyCalculator(OrderFilterInterface<Price, Qty>& filter)
        : filter_(filter) {}

    ~OrderMinQtyCalculator() override = default;

    // Возвращает пару: {количество, успешность вычисления}
    std::pair<bool, Qty> Calculate(Price price) override {
        if (!IsAligned(price, filter_.tick_size)) {
            return {false, Qty{0}};  // Цена не соответствует шагу цены
        }

        Qty min_qty_by_amount = filter_.min_order_amt / price;
        Qty raw_qty     = std::max(static_cast<Price>(filter_.min_order_qty),
                                   min_qty_by_amount);
        Qty stepped_qty = RoundUpToStep(raw_qty, filter_.base_precission);

        if (stepped_qty > filter_.max_order_qty) {
            return {false, Qty{0}};  // Превышено максимальное количество
        }

        typename OrderFilterInterface<Price, Qty>::Amount notional =
            stepped_qty * price;
        if (notional < filter_.min_order_amt ||
            notional > filter_.max_order_amt) {
            return {false,
                    Qty{0}};  // Номинал не попадает в допустимый диапазон
        }

        return {true, stepped_qty};
    }
};
};  // namespace spot
};  // namespace bybit
};  // namespace aoe