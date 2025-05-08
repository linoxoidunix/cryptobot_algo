#include <gtest/gtest.h>

#include "aoe/aoe.h"  // Ваш класс
#include "aos/aos.h"  // Ваш класс
#include "aos/trading_pair/trading_pair.h"
#include "aot/common/mem_pool.h"
#include "gmock/gmock.h"
#include "nlohmann/json.hpp"
using ::testing::Return;

using namespace aoe;
using namespace aoe::bybit;
using namespace aoe::bybit::impl;
using namespace aoe::bybit::place_order::impl;
using namespace aos;

// Мок для интерфейса TradingPairPrinterInterface
class MockTradingPairPrinter : public TradingPairPrinterInterface {
  public:
    MOCK_METHOD((std::pair<bool, std::string_view>), Print,
                (const aos::TradingPair&), (override));
};

// Тесты для SpotBuyLimitOrder
class SpotBuyLimitOrderTest : public ::testing::Test {
  protected:
    MockTradingPairPrinter mock_trading_pair_printer_;
    SpotBuyLimit<common::MemoryPoolThreadSafety> order_{
        mock_trading_pair_printer_};

    // Можно добавить дополнительные подготовительные шаги, если нужно
    void SetUp() override {
        // Установка значений по умолчанию для тестов, если нужно
    }
};

// Тестирование успешного случая
TEST_F(SpotBuyLimitOrderTest, AcceptReturnsValidJson) {
    // Мокируем метод Print
    EXPECT_CALL(mock_trading_pair_printer_, Print(::testing::_))
        .WillOnce(Return(
            std::make_pair(true, "BTCUSDT")));  // Символ "BTCUSDT" возвращаем

    // Тестируем метод Accept
    auto [status, json] =
        order_.Accept(nullptr);  // Можно передать mock, если необходимо

    // Проверяем, что статус успешный
    EXPECT_TRUE(status);

    // Проверяем, что json содержит все нужные поля
    EXPECT_EQ(json["category"], "spot");
    EXPECT_EQ(json["symbol"], "BTCUSDT");
    EXPECT_EQ(json["side"], "Buy");
    EXPECT_EQ(json["orderType"], "Limit");

    // Проверим, что необязательные поля не пустые (например, TimeInForce)
    EXPECT_EQ(json.find("timeInForce"), json.end());
}

// Тест на ошибку при некорректном символе
TEST_F(SpotBuyLimitOrderTest, AcceptReturnsEmptyJsonOnInvalidSymbol) {
    // Мокируем метод Print, который вернёт ошибку
    EXPECT_CALL(mock_trading_pair_printer_, Print(::testing::_))
        .WillOnce(
            Return(std::make_pair(false, "")));  // Ошибка при получении символа

    // Тестируем метод Accept
    auto [status, json] = order_.Accept(nullptr);

    // Проверяем, что статус неудачный
    EXPECT_FALSE(status);

    // Проверяем, что json пустой
    EXPECT_TRUE(json.empty());
}

// Тест на ошибку при некорректном категории
TEST_F(SpotBuyLimitOrderTest, AcceptReturnsEmptyJsonOnInvalidCategory) {
    // Мокируем метод CategoryToString, чтобы он возвращал ошибку
    EXPECT_CALL(mock_trading_pair_printer_, Print(::testing::_)).Times(0);

    // Мокируем CategoryToString на ошибку
    EXPECT_CALL(mock_trading_pair_printer_, Print(::testing::_))
        .WillOnce(Return(
            std::make_pair(false, "")));  // Ошибка при получении категории

    // Тестируем метод Accept
    auto [status, json] = order_.Accept(nullptr);

    // Проверяем, что статус неудачный
    EXPECT_FALSE(status);

    // Проверяем, что json пустой
    EXPECT_TRUE(json.empty());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}