#include <gtest/gtest.h>

#include <memory>

#include "aos/converters/trading_pair_to_big_string_view/trading_pair_to_big_string_view.h"

using namespace aos;
using namespace aos::impl;
TEST(TradingPairToBigStringViewTest, ConvertValidPair) {
    TradingPairToBigStringView converter;

    // Проверяем правильность конвертации для существующих торговых пар
    auto resultBTC = converter.Convert(TradingPair::kBTCUSDT);
    EXPECT_TRUE(resultBTC.first);
    EXPECT_EQ(resultBTC.second, "BTCUSDT");

    auto resultETH = converter.Convert(TradingPair::kETHUSDT);
    EXPECT_TRUE(resultETH.first);
    EXPECT_EQ(resultETH.second, "ETHUSDT");
}

TEST(TradingPairToBigStringViewTest, ConvertInvalidPair) {
    TradingPairToBigStringView converter;

    // Проверяем, что при передаче неверной торговой пары результат будет false
    TradingPair invalidPair = static_cast<TradingPair>(100);  // Пример несуществующей пары
    auto result = converter.Convert(invalidPair);

    EXPECT_FALSE(result.first);
    EXPECT_EQ(result.second, std::string_view{});
}

TEST(TradingPairToBigStringViewTest, ConvertCountPair) {
    TradingPairToBigStringView converter;

    // Проверяем, что kCount не существует в словаре и возвращает false
    auto result = converter.Convert(TradingPair::kCount);
    EXPECT_FALSE(result.first);
    EXPECT_EQ(result.second, std::string_view{});
}


int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}