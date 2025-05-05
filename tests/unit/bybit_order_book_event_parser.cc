#include <gtest/gtest.h>

#include <memory>

#include "aoe/aoe.h"
#include "aos/aos.h"
#include "aot/common/mem_pool.h"

TEST(OrderBookEventParserTest, ParsesBybitSnapshotCorrectly) {
    using Price = double;
    using Qty   = double;

    aos::impl::TradingPairFactoryTest trading_pair_factory;
    aoe::bybit::impl::OrderBookEventParser<Price, Qty,
                                           common::MemoryPoolThreadSafety>
        event_parser(10, trading_pair_factory);

    const std::string json = R"({
        "topic":"orderbook.50.BTCUSDT",
        "ts":1746477045779,
        "type":"snapshot",
        "data":{
            "s":"BTCUSDT",
            "b":[["88113.97","0.0001"],["88083.3","0.005152"]],
            "a":[["88396.98","3.120226"],["88400","0.069897"]],
            "u":679109,
            "seq":1651927344
        },
        "cts":1746477045494
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived =
        dynamic_cast<aoe::bybit::impl::OrderBookSnapshotEventDefault<
            Price, Qty, common::MemoryPoolThreadSafety>*>(event.get());
    ASSERT_NE(derived, nullptr);
    // Проверим поля
    EXPECT_EQ(event->UpdateId(), 679109);
    ASSERT_EQ(event->TradingPair(), common::TradingPair(2, 1));

    const auto& bids = event->Bids();
    const auto& asks = event->Asks();

    ASSERT_EQ(bids.size(), 2);
    EXPECT_DOUBLE_EQ(bids[0].price, 88113.97);
    EXPECT_DOUBLE_EQ(bids[0].qty, 0.0001);

    ASSERT_EQ(asks.size(), 2);
    EXPECT_DOUBLE_EQ(asks[0].price, 88396.98);
    EXPECT_DOUBLE_EQ(asks[0].qty, 3.120226);
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}