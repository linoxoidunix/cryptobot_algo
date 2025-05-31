#include <gtest/gtest.h>

#include "aoe/bybit/order_filter/trading_pair_order_filter.h"
#include "aoe/bybit/order_min_qty_calculator/calculator.h"

namespace aoe {
namespace bybit {
namespace spot {

TEST(OrderMinQtyCalculatorTest, ValidPriceAlignedWithinRange) {
    OrderFilterBTCUSDT filter;
    OrderMinQtyCalculator<double, double> calc(filter);

    auto [ok, qty] = calc.Calculate(100.0);  // цена кратна 0.1, min_amount = 5
    EXPECT_TRUE(ok);
    EXPECT_GE(qty, filter.min_order_qty);
    EXPECT_LE(qty, filter.max_order_qty);
    EXPECT_GE(qty * 100.0, filter.min_order_amt);
    EXPECT_LE(qty * 100.0, filter.max_order_amt);
}

TEST(OrderMinQtyCalculatorTest, PriceNotAligned) {
    OrderFilterBTCUSDT filter;
    OrderMinQtyCalculator<double, double> calc(filter);

    auto [ok, qty] = calc.Calculate(100.07);  // не кратна 0.1
    EXPECT_FALSE(ok);
    EXPECT_EQ(qty, 0.0);
}

TEST(OrderMinQtyCalculatorTest, NotionalBelowMinimum) {
    OrderFilterBTCUSDT filter;
    OrderMinQtyCalculator<double, double> calc(filter);

    auto [ok, qty] = calc.Calculate(0.1);  // min_amount = 5, значит qty > 50
    EXPECT_TRUE(ok);
    EXPECT_GE(qty * 0.1, filter.min_order_amt);
}

TEST(OrderMinQtyCalculatorTest, QtyExceedsMaximum) {
    OrderFilterBTCUSDT filter;
    OrderMinQtyCalculator<double, double> calc(filter);

    auto [ok, qty] =
        calc.Calculate(0.0001);  // Чтобы получить notional = 5, qty > 50_000
    EXPECT_FALSE(ok);            // qty > max_order_qty (83.0)
    EXPECT_EQ(qty, 0.0);
}

TEST(OrderMinQtyCalculatorTest, NotionalExceedsMaximum) {
    OrderFilterBTCUSDT filter;
    OrderMinQtyCalculator<double, double> calc(filter);

    auto [ok, qty] =
        calc.Calculate(100000.0);  // qty будет минимальное, но notional = qty *
                                   // price слишком большое
    EXPECT_FALSE(ok);
    EXPECT_EQ(qty, 0.0);
}

}  // namespace spot
}  // namespace bybit
}  // namespace aoe
