#include "aoe/bybit/parser/json/ws/execution_response/bybit_execution_event_parser.h"

#include <gtest/gtest.h>

#include <memory>

#include "aoe/bybit/execution_event/types.h"
#include "aoe/bybit/execution_watcher/execution_watcher.h"
#include "aos/aos.h"
#include "aos/trading_pair_factory/trading_pair_factory.h"
#include "aot/common/mem_pool.h"

using PositionT = int;  // Или ваша реальная структура позиции
using Parser =
    aoe::bybit::impl::BybitExecutionEventParser<common::MemoryPoolThreadSafety,
                                                PositionT>;

class TradingPairFactoryFake : public aos::TradingPairFactoryInterface {
  public:
    std::pair<bool, common::TradingPair> Produce(
        std::string_view trading_pair) override {
        if (trading_pair == "BTCUSDT") return {true, common::TradingPair{2, 1}};
        return {false, {}};
    }
};

TEST(BybitExecutionEventParserTest, ParseSpotBuySuccess) {
    // JSON с полными корректными полями
    std::string json = R"({
        "data": [
            {
                "category": "spot",
                "symbol": "BTCUSDT",
                "side": "Buy",
                "execFee": "0.005061",
                "execPrice": "0.3374",
                "execQty": "25",
                "execValue": "8.435",
                "orderId": "123123"
            }
        ]
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    TradingPairFactoryFake factory;
    Parser event_parser(/*pool_size=*/10, factory);

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_TRUE(ok);
    ASSERT_NE(event, nullptr);
    auto* derived = dynamic_cast<aoe::bybit::ExecutionEventSpotBuyDefault<
        common::MemoryPoolThreadSafety, PositionT>*>(event.get());
    ASSERT_NE(derived, nullptr);
    ASSERT_EQ(event->ExchangeId(), common::ExchangeId::kBybit);
    ASSERT_EQ(event->ExecPrice(), 0.3374);
    ASSERT_EQ(event->ExecQty(), 25);
    ASSERT_EQ(event->ExecQtyAbs(), 25);
    ASSERT_EQ(event->ExecValue(), 8.435);
    ASSERT_EQ(event->OrderId(), 123123);
    ASSERT_EQ(event->Fee(), 0.005061);
    ASSERT_EQ(event->TradingPair(), common::TradingPair(2, 1));

    // Дополнительно можно проверить значения, если у event есть геттеры
    // Например: EXPECT_DOUBLE_EQ(event->GetExecPrice(), 0.3374);
}

TEST(BybitExecutionEventParserTest, ParseSpotSellSuccess) {
    std::string json = R"({
        "data": [
            {
                "category": "spot",
                "symbol": "BTCUSDT",
                "side": "Sell",
                "execFee": "0.001",
                "execPrice": "100.0",
                "execQty": "0.5",
                "execValue": "50.0",
                "orderId": "456"
            }
        ]
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    TradingPairFactoryFake factory;
    Parser event_parser(10, factory);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_TRUE(ok);
    ASSERT_NE(event, nullptr);

    auto* derived = dynamic_cast<aoe::bybit::ExecutionEventSpotSellDefault<
        common::MemoryPoolThreadSafety, PositionT>*>(event.get());
    ASSERT_NE(derived, nullptr);
    ASSERT_EQ(event->ExecQty(), 0.5);
    ASSERT_EQ(event->ExecValue(), 50.0);
    ASSERT_EQ(event->Fee(), 0.001);
    ASSERT_EQ(event->OrderId(), 456);
    ASSERT_EQ(event->ExchangeId(), common::ExchangeId::kBybit);
}

TEST(BybitExecutionEventParserTest, ParseLinearBuySuccess) {
    std::string json = R"({
        "data": [
            {
                "category": "linear",
                "symbol": "BTCUSDT",
                "side": "Buy",
                "execFee": "0.002",
                "execPrice": "30000",
                "execQty": "0.1",
                "execValue": "3000",
                "orderId": "678"
            }
        ]
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    TradingPairFactoryFake factory;
    Parser event_parser(10, factory);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_TRUE(ok);
    ASSERT_NE(event, nullptr);

    auto* derived = dynamic_cast<aoe::bybit::ExecutionEventLinearBuyDefault<
        common::MemoryPoolThreadSafety, PositionT>*>(event.get());
    ASSERT_NE(derived, nullptr);
    ASSERT_EQ(event->ExecQty(), -0.1);  // In linear buy qty is negative
    ASSERT_EQ(event->ExecValue(), 3000.0);
    ASSERT_EQ(event->Fee(), 0.002);
    ASSERT_EQ(event->OrderId(), 678);
    ASSERT_EQ(event->ExchangeId(), common::ExchangeId::kBybit);
}

TEST(BybitExecutionEventParserTest, ParseLinearSellSuccess) {
    std::string json = R"({
        "data": [
            {
                "category": "linear",
                "symbol": "BTCUSDT",
                "side": "Sell",
                "execFee": "0.003",
                "execPrice": "32000",
                "execQty": "0.2",
                "execValue": "6400",
                "orderId": "890"
            }
        ]
    })";

    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);
    ASSERT_EQ(doc.error(), simdjson::SUCCESS);

    TradingPairFactoryFake factory;
    Parser event_parser(10, factory);
    auto [ok, event] = event_parser.ParseAndCreate(doc.value());

    EXPECT_TRUE(ok);
    ASSERT_NE(event, nullptr);

    auto* derived = dynamic_cast<aoe::bybit::ExecutionEventLinearSellDefault<
        common::MemoryPoolThreadSafety, PositionT>*>(event.get());
    ASSERT_NE(derived, nullptr);
    ASSERT_EQ(event->ExecQty(), -0.2);  // In linear sell qty is also negative
    ASSERT_EQ(event->ExecValue(), 6400.0);
    ASSERT_EQ(event->Fee(), 0.003);
    ASSERT_EQ(event->OrderId(), 890);
    ASSERT_EQ(event->ExchangeId(), common::ExchangeId::kBybit);
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}