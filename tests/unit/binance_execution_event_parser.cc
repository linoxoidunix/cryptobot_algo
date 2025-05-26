#include <gtest/gtest.h>

#include <memory>

#include "aoe/binance/execution_event/types.h"
#include "aoe/binance/parser/json/ws/execution_response/execution_event_parser.h"
#include "aos/aos.h"
#include "aos/trading_pair/trading_pair.h"
#include "aot/common/mem_pool.h"

using PositionT  = int;  // Или ваша реальная структура позиции
using SpotParser = aoe::binance::spot::impl::ExecutionEventParser<
    common::MemoryPoolThreadSafety, PositionT>;
using FuturesParser = aoe::binance::futures::impl::ExecutionEventParser<
    common::MemoryPoolThreadSafety, PositionT>;

TEST(BinanceExecutionEventParserTest, ParseSpotBuySuccess) {
    // JSON с полными корректными полями
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
        "x": "TRADE",
        "X": "NEW",
        "r": "NONE",
        "i": 4293153,
        "l": "0.05",
        "z": "0.00000000",
        "L": "123456.789",
        "n": "0.005061",
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
        "Y": "6400",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    SpotParser event_parser(/*pool_size=*/10);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<aoe::binance::ExecutionEventSpotBuyDefault<
        common::MemoryPoolThreadSafety, PositionT>*>(event.get());
    ASSERT_NE(derived, nullptr);
    ASSERT_EQ(event->ExchangeId(), common::ExchangeId::kBinance);
    ASSERT_EQ(event->ExecPrice(), 123456.789);
    ASSERT_EQ(event->ExecQty(), 0.05);
    ASSERT_EQ(event->ExecQtyAbs(), 0.05);
    ASSERT_EQ(event->ExecValue(), 6400);
    ASSERT_EQ(event->OrderId(), 1234567890);
    ASSERT_EQ(event->Fee(), 0.005061);
    ASSERT_EQ(event->TradingPair(), aos::TradingPair::kBTCUSDT);

    // Дополнительно можно проверить значения, если у event есть геттеры
    // Например: EXPECT_DOUBLE_EQ(event->GetExecPrice(), 0.3374);
}

TEST(BinanceExecutionEventParserTest, ParseSpotSellSuccess) {
    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "SELL",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "TRADE",
        "X": "PENDING_NEW",
        "r": "NONE",
        "i": 4293153,
        "l": "0.05",
        "z": "0.00000000",
        "L": "123456.789",
        "n": "0.001",
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
        "Y": "6400",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    SpotParser event_parser(10);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_TRUE(ok);
    ASSERT_NE(event, nullptr);

    auto* derived = dynamic_cast<aoe::binance::ExecutionEventSpotSellDefault<
        common::MemoryPoolThreadSafety, PositionT>*>(event.get());
    ASSERT_NE(derived, nullptr);
    ASSERT_EQ(event->ExecQty(), 0.05);
    ASSERT_EQ(event->ExecQtyAbs(), 0.05);
    ASSERT_EQ(event->ExecValue(), 6400);
    ASSERT_EQ(event->Fee(), 0.001);
    ASSERT_EQ(event->OrderId(), 1234567890);
    ASSERT_EQ(event->ExchangeId(), common::ExchangeId::kBinance);
}

TEST(BinanceExecutionEventParserTest, ParseFuturesBuySuccess) {
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
        "x": "TRADE",
        "X": "PENDING_NEW",
        "r": "NONE",
        "i": 4293153,
        "l": "0.05",
        "z": "0.00000000",
        "L": "123456.789",
        "n": "0.002",
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
        "Y": "6400",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    FuturesParser event_parser(10);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_TRUE(ok);
    ASSERT_NE(event, nullptr);

    auto* derived = dynamic_cast<aoe::binance::ExecutionEventFuturesBuyDefault<
        common::MemoryPoolThreadSafety, PositionT>*>(event.get());
    ASSERT_NE(derived, nullptr);
    ASSERT_EQ(event->ExecQty(), -0.05);  // In linear buy qty is negative
    ASSERT_EQ(event->ExecQtyAbs(), 0.05);
    ASSERT_EQ(event->ExecValue(), 6400.0);
    ASSERT_EQ(event->Fee(), 0.002);
    ASSERT_EQ(event->OrderId(), 1234567890);
    ASSERT_EQ(event->ExchangeId(), common::ExchangeId::kBinance);
}

TEST(BinanceExecutionEventParserTest, ParseFuturesSellSuccess) {
    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "SELL",
        "o": "LIMIT",
        "f": "GTC",
        "q": "1.00000000",
        "p": "0.10264410",
        "P": "0.00000000",
        "F": "0.00000000",
        "g": -1,
        "C": "",
        "x": "TRADE",
        "X": "PENDING_NEW",
        "r": "NONE",
        "i": 4293153,
        "l": "0.05",
        "z": "0.00000000",
        "L": "123456.789",
        "n": "0.003",
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
        "Y": "6400",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    FuturesParser event_parser(10);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_TRUE(ok);
    ASSERT_NE(event, nullptr);

    auto* derived = dynamic_cast<aoe::binance::ExecutionEventFuturesSellDefault<
        common::MemoryPoolThreadSafety, PositionT>*>(event.get());
    ASSERT_NE(derived, nullptr);
    ASSERT_EQ(event->ExecQty(), -0.05);  // In futures sell qty is also negative
    ASSERT_EQ(event->ExecQtyAbs(), 0.05);
    ASSERT_EQ(event->ExecValue(), 6400.0);
    ASSERT_EQ(event->Fee(), 0.003);
    ASSERT_EQ(event->OrderId(), 1234567890);
    ASSERT_EQ(event->ExchangeId(), common::ExchangeId::kBinance);
}

TEST(BinanceExecutionEventParserTest, ParseFuturesSellSuccessIfEventNotTrade) {
    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "SELL",
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
        "l": "0.05",
        "z": "0.00000000",
        "L": "123456.789",
        "n": "0.003",
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
        "Y": "6400",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    FuturesParser event_parser(10);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_FALSE(ok);
}

TEST(BinanceExecutionEventParserTest, ParseSpotSellSuccessIfEventNotTrade) {
    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "SELL",
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
        "l": "0.05",
        "z": "0.00000000",
        "L": "123456.789",
        "n": "0.003",
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
        "Y": "6400",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    SpotParser event_parser(10);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_FALSE(ok);
}

TEST(BinanceExecutionEventParserTest, ParseEmptyDoc) {
    std::string json = R"({
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    SpotParser event_parser(10);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_FALSE(ok);
}

TEST(BinanceExecutionEventParserTest, ParseMissedEventTypeSuccess) {
    std::string json = R"({
        "E": 1499405658658,
        "s": "BTCUSDT",
        "c": "1234567890",
        "S": "SELL",
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
        "l": "0.05",
        "z": "0.00000000",
        "L": "123456.789",
        "n": "0.003",
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
        "Y": "6400",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    SpotParser event_parser(10);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_FALSE(ok);
}

TEST(BinanceExecutionEventParserTest, ParseMissedSymbolSuccess) {
    std::string json = R"({
        "e": "executionReport",
        "E": 1499405658658,
        "c": "1234567890",
        "S": "SELL",
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
        "l": "0.05",
        "z": "0.00000000",
        "L": "123456.789",
        "n": "0.003",
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
        "Y": "6400",
        "Q": "0.00000000",
        "W": 1499405658657,
        "V": "NONE"
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    FuturesParser event_parser(10);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_FALSE(ok);
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}