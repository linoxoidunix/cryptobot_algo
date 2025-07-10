#include <iostream>

#include "aos/converters/trading_pair_to_tickers/converter.h"
#include "aos/logger/mylog.h"
#include "aos/trading_pair_triangle/trading_pair_triangle.h"

int main() {
    {
        auto t = aos::MakeTriangle(aos::spot::Buy(aos::TradingPair::kETHUSDT),
                                   aos::spot::Sell(aos::TradingPair::kETHBTC),
                                   aos::spot::Sell(aos::TradingPair::kBTCUSDT));
        logi("{}", t);
    }
    fmtlog::poll();
    return 0;
}