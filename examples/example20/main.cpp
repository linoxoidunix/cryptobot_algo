#include "aoe/bybit/execution_event/types.h"
#include "aoe/bybit/execution_watcher/execution_watcher.h"
#include "aoe/bybit/parser/json/ws/execution_response/execution_event_parser.h"
#include "aos/aos.h"
#include "aos/common/mem_pool.h"

struct DummyPosition {};
using PositionT = aos::impl::NetPositionDefault<double, double>;

int main() {
    try {
        using Parser = aoe::bybit::impl::ExecutionEventParser<
            common::MemoryPoolThreadSafety, PositionT>;

        std::string json = R"({
        "data": [
            { "category": "spot", "symbol": "BTCUSDT", "side": "Buy", "execFee": "0.005061", "execPrice": "0.3374", "execQty": "25", "execValue": "8.435", "orderId": "123123"}
        ]
    })";
        // std::string json = R"({
        //     "data": [
        //         { "category": "spot", "side": "Buy" },
        //         { "category": "linear", "side": "Sell" }
        //     ]
        // })";
        simdjson::ondemand::parser parser;
        simdjson::padded_string padded(json);
        auto doc = parser.iterate(padded);

        Parser event_parser(/*pool_size=*/100);
        // event_parser.RegisterFromConfig(
        //     aoe::bybit::impl::GetDefaultEventConfig<common::MemoryPoolThreadSafety,
        //                                             PositionT>());

        auto [ok, event] = event_parser.ParseAndCreate(doc.value());
        if (ok && event) {
            std::cout << "found event.\n";
        } else {
            std::cout << "No matching event found.\n";
        }
    } catch (...) {
        loge("found unexpected error");
    }
    fmtlog::poll();
    return 0;
}