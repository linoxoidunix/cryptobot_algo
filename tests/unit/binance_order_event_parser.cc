#include <gtest/gtest.h>

#include <memory>

#include "aoe/binance/parser/json/ws/order_response/order_event_parser.h"
#include "aos/common/mem_pool.h"

TEST(OrderEventParserTest, ParsesBinanceExecutionReportNewEvent) {
    using Price = double;
    using Qty   = double;

    aoe::binance::impl::OrderEventParser<common::MemoryPoolThreadSafety>
        event_parser(10);

    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "BUY",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "NEW",
        "X": "NEW",
        "r": "NONE",
        "i": 4293153,
        "l": "0.00000000",
        "z": "0.00000000",
        "L": "0.00000000",
        "n": "0",
        "N": null,
        "T": 1499405658657,
        "t": -1,
        "v": 3,
        "I": 8641984,
        "w": true,
        "m": false,
        "M": false,
        "O": 1499405658657,
        "Z": "0.00000000",
        "Y": "0.00000000",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<
        aoe::binance::OrderEventNew<common::MemoryPoolThreadSafety>*>(
        event.get());
    ASSERT_NE(derived, nullptr);
    // Проверка корректности полей
    EXPECT_EQ(event->OrderId(), 1234567890);                      // i
    EXPECT_DOUBLE_EQ(event->Price(), 0.10264410);                 // p
    EXPECT_DOUBLE_EQ(event->OrderQty(), 1.0);                     // q
    EXPECT_DOUBLE_EQ(event->CumExecQty(), 0.0);                   // z
    EXPECT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);  // s
}

TEST(OrderEventParserTest, ParsesBinanceExecutionReportPendingNewEvent) {
    using Price = double;
    using Qty   = double;

    aoe::binance::impl::OrderEventParser<common::MemoryPoolThreadSafety>
        event_parser(10);

    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "BUY",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "NEW",
        "X": "PENDING_NEW",
        "r": "NONE",
        "i": 4293153,
        "l": "0.00000000",
        "z": "0.00000000",
        "L": "0.00000000",
        "n": "0",
        "N": null,
        "T": 1499405658657,
        "t": -1,
        "v": 3,
        "I": 8641984,
        "w": true,
        "m": false,
        "M": false,
        "O": 1499405658657,
        "Z": "0.00000000",
        "Y": "0.00000000",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<
        aoe::binance::OrderEventPendingNew<common::MemoryPoolThreadSafety>*>(
        event.get());
    ASSERT_NE(derived, nullptr);
    // Проверка корректности полей
    EXPECT_EQ(event->OrderId(), 1234567890);                      // i
    EXPECT_DOUBLE_EQ(event->Price(), 0.10264410);                 // p
    EXPECT_DOUBLE_EQ(event->OrderQty(), 1.0);                     // q
    EXPECT_DOUBLE_EQ(event->CumExecQty(), 0.0);                   // z
    EXPECT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);  // s
}

TEST(OrderEventParserTest, ParsesBinanceExecutionReportPartiatillyFilledEvent) {
    using Price = double;
    using Qty   = double;

    aoe::binance::impl::OrderEventParser<common::MemoryPoolThreadSafety>
        event_parser(10);

    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "BUY",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "NEW",
        "X": "PARTIALLY_FILLED",
        "r": "NONE",
        "i": 4293153,
        "l": "0.00000000",
        "z": "0.00000000",
        "L": "0.00000000",
        "n": "0",
        "N": null,
        "T": 1499405658657,
        "t": -1,
        "v": 3,
        "I": 8641984,
        "w": true,
        "m": false,
        "M": false,
        "O": 1499405658657,
        "Z": "0.00000000",
        "Y": "0.00000000",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<aoe::binance::OrderEventPartiallyFilled<
        common::MemoryPoolThreadSafety>*>(event.get());
    ASSERT_NE(derived, nullptr);
    // Проверка корректности полей
    EXPECT_EQ(event->OrderId(), 1234567890);                      // i
    EXPECT_DOUBLE_EQ(event->Price(), 0.10264410);                 // p
    EXPECT_DOUBLE_EQ(event->OrderQty(), 1.0);                     // q
    EXPECT_DOUBLE_EQ(event->CumExecQty(), 0.0);                   // z
    EXPECT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);  // s
}

TEST(OrderEventParserTest, ParsesBinanceExecutionReportFilledEvent) {
    using Price = double;
    using Qty   = double;

    aoe::binance::impl::OrderEventParser<common::MemoryPoolThreadSafety>
        event_parser(10);

    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "BUY",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "NEW",
        "X": "FILLED",
        "r": "NONE",
        "i": 4293153,
        "l": "0.00000000",
        "z": "0.00000000",
        "L": "0.00000000",
        "n": "0",
        "N": null,
        "T": 1499405658657,
        "t": -1,
        "v": 3,
        "I": 8641984,
        "w": true,
        "m": false,
        "M": false,
        "O": 1499405658657,
        "Z": "0.00000000",
        "Y": "0.00000000",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<
        aoe::binance::OrderEventFilled<common::MemoryPoolThreadSafety>*>(
        event.get());
    ASSERT_NE(derived, nullptr);
    // Проверка корректности полей
    EXPECT_EQ(event->OrderId(), 1234567890);                      // i
    EXPECT_DOUBLE_EQ(event->Price(), 0.10264410);                 // p
    EXPECT_DOUBLE_EQ(event->OrderQty(), 1.0);                     // q
    EXPECT_DOUBLE_EQ(event->CumExecQty(), 0.0);                   // z
    EXPECT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);  // s
}

TEST(OrderEventParserTest, ParsesBinanceExecutionReportCancelledEvent) {
    using Price = double;
    using Qty   = double;

    aoe::binance::impl::OrderEventParser<common::MemoryPoolThreadSafety>
        event_parser(10);

    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "BUY",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "NEW",
        "X": "CANCELED",
        "r": "NONE",
        "i": 4293153,
        "l": "0.00000000",
        "z": "0.00000000",
        "L": "0.00000000",
        "n": "0",
        "N": null,
        "T": 1499405658657,
        "t": -1,
        "v": 3,
        "I": 8641984,
        "w": true,
        "m": false,
        "M": false,
        "O": 1499405658657,
        "Z": "0.00000000",
        "Y": "0.00000000",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<
        aoe::binance::OrderEventCancelled<common::MemoryPoolThreadSafety>*>(
        event.get());
    ASSERT_NE(derived, nullptr);
    // Проверка корректности полей
    EXPECT_EQ(event->OrderId(), 1234567890);                      // i
    EXPECT_DOUBLE_EQ(event->Price(), 0.10264410);                 // p
    EXPECT_DOUBLE_EQ(event->OrderQty(), 1.0);                     // q
    EXPECT_DOUBLE_EQ(event->CumExecQty(), 0.0);                   // z
    EXPECT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);  // s
}

TEST(OrderEventParserTest, ParsesBinanceExecutionReportPendingCancellEvent) {
    using Price = double;
    using Qty   = double;

    aoe::binance::impl::OrderEventParser<common::MemoryPoolThreadSafety>
        event_parser(10);

    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "BUY",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "NEW",
        "X": "PENDING_CANCEL",
        "r": "NONE",
        "i": 4293153,
        "l": "0.00000000",
        "z": "0.00000000",
        "L": "0.00000000",
        "n": "0",
        "N": null,
        "T": 1499405658657,
        "t": -1,
        "v": 3,
        "I": 8641984,
        "w": true,
        "m": false,
        "M": false,
        "O": 1499405658657,
        "Z": "0.00000000",
        "Y": "0.00000000",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<
        aoe::binance::OrderEventPendingCancel<common::MemoryPoolThreadSafety>*>(
        event.get());
    ASSERT_NE(derived, nullptr);
    // Проверка корректности полей
    EXPECT_EQ(event->OrderId(), 1234567890);                      // i
    EXPECT_DOUBLE_EQ(event->Price(), 0.10264410);                 // p
    EXPECT_DOUBLE_EQ(event->OrderQty(), 1.0);                     // q
    EXPECT_DOUBLE_EQ(event->CumExecQty(), 0.0);                   // z
    EXPECT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);  // s
}

TEST(OrderEventParserTest, ParsesBinanceExecutionReportRejectedEvent) {
    using Price = double;
    using Qty   = double;

    aoe::binance::impl::OrderEventParser<common::MemoryPoolThreadSafety>
        event_parser(10);

    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "BUY",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "NEW",
        "X": "REJECTED",
        "r": "NONE",
        "i": 4293153,
        "l": "0.00000000",
        "z": "0.00000000",
        "L": "0.00000000",
        "n": "0",
        "N": null,
        "T": 1499405658657,
        "t": -1,
        "v": 3,
        "I": 8641984,
        "w": true,
        "m": false,
        "M": false,
        "O": 1499405658657,
        "Z": "0.00000000",
        "Y": "0.00000000",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<
        aoe::binance::OrderEventRejected<common::MemoryPoolThreadSafety>*>(
        event.get());
    ASSERT_NE(derived, nullptr);
    // Проверка корректности полей
    EXPECT_EQ(event->OrderId(), 1234567890);                      // i
    EXPECT_DOUBLE_EQ(event->Price(), 0.10264410);                 // p
    EXPECT_DOUBLE_EQ(event->OrderQty(), 1.0);                     // q
    EXPECT_DOUBLE_EQ(event->CumExecQty(), 0.0);                   // z
    EXPECT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);  // s
}

TEST(OrderEventParserTest, ParsesBinanceExecutionReportExpiredEvent) {
    using Price = double;
    using Qty   = double;

    aoe::binance::impl::OrderEventParser<common::MemoryPoolThreadSafety>
        event_parser(10);

    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "BUY",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "NEW",
        "X": "EXPIRED",
        "r": "NONE",
        "i": 4293153,
        "l": "0.00000000",
        "z": "0.00000000",
        "L": "0.00000000",
        "n": "0",
        "N": null,
        "T": 1499405658657,
        "t": -1,
        "v": 3,
        "I": 8641984,
        "w": true,
        "m": false,
        "M": false,
        "O": 1499405658657,
        "Z": "0.00000000",
        "Y": "0.00000000",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<
        aoe::binance::OrderEventExpired<common::MemoryPoolThreadSafety>*>(
        event.get());
    ASSERT_NE(derived, nullptr);
    // Проверка корректности полей
    EXPECT_EQ(event->OrderId(), 1234567890);                      // i
    EXPECT_DOUBLE_EQ(event->Price(), 0.10264410);                 // p
    EXPECT_DOUBLE_EQ(event->OrderQty(), 1.0);                     // q
    EXPECT_DOUBLE_EQ(event->CumExecQty(), 0.0);                   // z
    EXPECT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);  // s
}

TEST(OrderEventParserTest, ParsesBinanceExecutionReportExpiredInMatchEvent) {
    using Price = double;
    using Qty   = double;

    aoe::binance::impl::OrderEventParser<common::MemoryPoolThreadSafety>
        event_parser(10);

    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "BUY",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "NEW",
        "X": "EXPIRED_IN_MATCH",
        "r": "NONE",
        "i": 4293153,
        "l": "0.00000000",
        "z": "0.00000000",
        "L": "0.00000000",
        "n": "0",
        "N": null,
        "T": 1499405658657,
        "t": -1,
        "v": 3,
        "I": 8641984,
        "w": true,
        "m": false,
        "M": false,
        "O": 1499405658657,
        "Z": "0.00000000",
        "Y": "0.00000000",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    ASSERT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<aoe::binance::OrderEventExpiredInMatch<
        common::MemoryPoolThreadSafety>*>(event.get());
    ASSERT_NE(derived, nullptr);
    // Проверка корректности полей
    EXPECT_EQ(event->OrderId(), 1234567890);                      // i
    EXPECT_DOUBLE_EQ(event->Price(), 0.10264410);                 // p
    EXPECT_DOUBLE_EQ(event->OrderQty(), 1.0);                     // q
    EXPECT_DOUBLE_EQ(event->CumExecQty(), 0.0);                   // z
    EXPECT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);  // s
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}