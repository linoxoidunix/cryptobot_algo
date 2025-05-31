#pragma once
#include "aoe/bybit/order_filter/i_order_filter.h"

namespace aoe {
namespace bybit {
namespace spot {
struct OrderFilterBTCUSDT : OrderFilterInterface<double, double> {
    constexpr OrderFilterBTCUSDT() {
        min_order_qty    = 0.000011;   // lotSizeFilter.minOrderQty
        max_order_qty    = 83.0;       // lotSizeFilter.maxOrderQty
        min_order_amt    = 5.0;        // lotSizeFilter.minOrderAmt
        max_order_amt    = 8000000.0;  // lotSizeFilter.maxOrderAmt
        base_precission  = 0.000001;   // lotSizeFilter.basePrecision
        quote_precission = 0.0000001;  // lotSizeFilter.quotePrecision
        tick_size        = 0.1;        // priceFilter.tickSize
    }
};
};  // namespace spot
};  // namespace bybit
};  // namespace aoe