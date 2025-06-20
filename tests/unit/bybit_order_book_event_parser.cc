#include <memory>

#include "aoe/bybit/parser/json/ws/order_book_response/parser.h"
#include "aos/common/mem_pool.h"
#include "gtest/gtest.h"

TEST(OrderBookEventParserTest, ParsesBybitSnapshotCorrectly) {
    using Price = double;
    using Qty   = double;

    aoe::bybit::impl::OrderBookEventParser<Price, Qty,
                                           common::MemoryPoolThreadSafety>
        event_parser(10);

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
    ASSERT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);

    const auto& bids = event->Bids();
    const auto& asks = event->Asks();

    ASSERT_EQ(bids.size(), 2);
    EXPECT_DOUBLE_EQ(bids[0].price, 88113.97);
    EXPECT_DOUBLE_EQ(bids[0].qty, 0.0001);

    ASSERT_EQ(asks.size(), 2);
    EXPECT_DOUBLE_EQ(asks[0].price, 88396.98);
    EXPECT_DOUBLE_EQ(asks[0].qty, 3.120226);
}

TEST(OrderBookEventParserTest, ParsesBybitDiffCorrectly) {
    using Price = double;
    using Qty   = double;

    aoe::bybit::impl::OrderBookEventParser<Price, Qty,
                                           common::MemoryPoolThreadSafety>
        event_parser(10);

    const std::string json = R"({
        "topic":"orderbook.50.BTCUSDT",
        "ts":1746554535898,
        "type":"delta",
        "data":{
            "s":"BTCUSDT",
            "b":[
                ["87885","0"],["87785","0"],["87685","0"],["87585","0"],["87485","0"],
                ["87385","0"],["87285","0"],["87185","0"],["87085","0"],
                ["86400","0.009499"],["86382.03","0.000485"],["86300","0.060469"],
                ["86242.94","0.005573"],["86200","68.859518"],["86165.4","0.000267"],
                ["86136.84","75"],["86122.77","0.000253"],["86092.57","0.000196"]
            ],
            "a":[
                ["88185","0"],["88285","0"],["88385","0"],["88485","0"],["88585","0"],
                ["88685","0"],["88785","0"],["88885","0"],["88985","0"],["89085","0"],
                ["89494.73","150"],["89500","0.000285"],["89508.47","0.000562"],
                ["89516.7","0.001122"],["89536.84","150"],["89553.75","0.000278"],
                ["89578.94","150"],["89599.92","0.003714"],["89600","0.000801"],
                ["89621.05","150"]
            ],
            "u":729815,
            "seq":1652145551
        },
        "cts":1746554535876
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);

    auto* derived = dynamic_cast<aoe::bybit::impl::OrderBookDiffEventDefault<
        Price, Qty, common::MemoryPoolThreadSafety>*>(event.get());
    ASSERT_NE(derived, nullptr);

    EXPECT_EQ(event->UpdateId(), 729815);
    ASSERT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);

    const auto& bids = event->Bids();
    const auto& asks = event->Asks();

    ASSERT_EQ(bids.size(), 18);
    EXPECT_DOUBLE_EQ(bids[9].price, 86400.0);
    EXPECT_DOUBLE_EQ(bids[9].qty, 0.009499);

    ASSERT_EQ(asks.size(), 20);
    EXPECT_DOUBLE_EQ(asks[10].price, 89494.73);
    EXPECT_DOUBLE_EQ(asks[10].qty, 150.0);
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}