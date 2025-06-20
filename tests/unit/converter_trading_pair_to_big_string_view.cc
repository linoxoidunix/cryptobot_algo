#include <memory>

#include "aos/converters/trading_pair_to_big_string_view/trading_pair_to_big_string_view.h"
#include "gtest/gtest.h"
using aos::TradingPair;

TEST(TradingPairToBigStringViewTest, ConvertValidPair) {
    // Проверяем правильность конвертации для существующих торговых пар
    auto result_btc =
        aos::impl::TradingPairToBigStringView::Convert(TradingPair::kBTCUSDT);
    EXPECT_TRUE(result_btc.first);
    EXPECT_EQ(result_btc.second, "BTCUSDT");

    auto result_eth =
        aos::impl::TradingPairToBigStringView::Convert(TradingPair::kETHUSDT);
    EXPECT_TRUE(result_eth.first);
    EXPECT_EQ(result_eth.second, "ETHUSDT");
}

TEST(TradingPairToBigStringViewTest, ConvertInvalidPair) {
    // Проверяем, что при передаче неверной торговой пары результат будет false
    auto invalid_pair =
        static_cast<aos::TradingPair>(100);  // Пример несуществующей пары
    auto result = aos::impl::TradingPairToBigStringView::Convert(invalid_pair);

    EXPECT_FALSE(result.first);
    EXPECT_EQ(result.second, std::string_view{});
}

TEST(TradingPairToBigStringViewTest, ConvertCountPair) {
    // Проверяем, что kCount не существует в словаре и возвращает false
    auto result = aos::impl::TradingPairToBigStringView::Convert(
        aos::TradingPair::kCount);
    EXPECT_FALSE(result.first);
    EXPECT_EQ(result.second, std::string_view{});
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}