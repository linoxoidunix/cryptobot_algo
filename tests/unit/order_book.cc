#include "aos/order_book/order_book.h"

#include <gtest/gtest.h>

#include "aoe/aoe.h"  // Ваш класс
#include "aos/aos.h"  // Ваш класс
#include "aos/order_book_level/order_book_level.h"
#include "aot/common/mem_pool.h"
#include "gmock/gmock.h"
using ::testing::Return;

using namespace aos;
using namespace aoe::bybit;
using namespace aoe::bybit::impl;
using namespace aos;

// Объявление типа OrderBook с подстановкой
using TestOrderBookInner = aos::OrderBookInner<
    double, double, common::MemoryPoolNotThreadSafety,
    std::unordered_map<double, OrderBookLevel<double, double>*>>;

TEST(OrderBookTest, BasicAddAndGetBBO) {
    TestOrderBookInner book{100};

    book.AddBidLevel(100.0, 5.0);
    book.AddAskLevel(101.0, 10.0);

    auto [has_bbo, bbo] = book.GetBBO();
    ASSERT_TRUE(has_bbo);
    EXPECT_DOUBLE_EQ(bbo.bid_price, 100.0);
    EXPECT_DOUBLE_EQ(bbo.bid_qty, 5.0);
    EXPECT_DOUBLE_EQ(bbo.ask_price, 101.0);
    EXPECT_DOUBLE_EQ(bbo.ask_qty, 10.0);
}

TEST(OrderBookTest, EmptyBBO) {
    TestOrderBookInner book{100};
    auto [has_bbo, bbo] = book.GetBBO();
    EXPECT_FALSE(has_bbo);
}

TEST(OrderBookTest, OverwriteBidLevel) {
    TestOrderBookInner book{100};
    book.AddBidLevel(100.0, 5.0);
    book.AddBidLevel(100.0, 8.0);  // перезапись

    book.AddAskLevel(101.0, 10.0);  // нужно, иначе GetBBO вернёт false

    auto [has_bbo, bbo] = book.GetBBO();
    ASSERT_TRUE(has_bbo);
    EXPECT_DOUBLE_EQ(bbo.bid_price, 100.0);
    EXPECT_DOUBLE_EQ(bbo.bid_qty, 8.0);  // новое значение
}

TEST(OrderBookTest, OverwriteAskLevel) {
    TestOrderBookInner book{100};
    book.AddBidLevel(100.0, 5.0);  // нужно для GetBBO

    book.AddAskLevel(101.0, 10.0);
    book.AddAskLevel(101.0, 3.0);  // перезапись

    auto [has_bbo, bbo] = book.GetBBO();
    ASSERT_TRUE(has_bbo);
    EXPECT_DOUBLE_EQ(bbo.ask_price, 101.0);
    EXPECT_DOUBLE_EQ(bbo.ask_qty, 3.0);
}

TEST(OrderBookTest, RemoveBidAndAsk) {
    TestOrderBookInner book{100};
    book.AddBidLevel(100.0, 5.0);
    book.AddAskLevel(101.0, 10.0);

    book.RemoveBidLevel(100.0);  // удаление
    book.RemoveAskLevel(101.0);  // удаление

    auto [has_bbo, _] = book.GetBBO();
    EXPECT_FALSE(has_bbo);
}

TEST(OrderBookTest, MultipleLevelsBBOSelection) {
    TestOrderBookInner book{100};
    book.AddBidLevel(99.0, 1.0);
    book.AddBidLevel(100.0, 2.0);  // best bid
    book.AddBidLevel(98.0, 3.0);

    book.AddAskLevel(102.0, 4.0);
    book.AddAskLevel(101.0, 5.0);  // best ask
    book.AddAskLevel(103.0, 6.0);

    auto [has_bbo, bbo] = book.GetBBO();
    ASSERT_TRUE(has_bbo);
    EXPECT_DOUBLE_EQ(bbo.bid_price, 100.0);
    EXPECT_DOUBLE_EQ(bbo.bid_qty, 2.0);
    EXPECT_DOUBLE_EQ(bbo.ask_price, 101.0);
    EXPECT_DOUBLE_EQ(bbo.ask_qty, 5.0);
}

TEST(OrderBookTest, RemoveOneSideExpectBBOFalse) {
    TestOrderBookInner book{100};
    book.AddBidLevel(100.0, 5.0);
    book.AddAskLevel(101.0, 10.0);

    book.RemoveAskLevel(101.0);  // удаление ask

    auto [has_bbo, _] = book.GetBBO();
    EXPECT_FALSE(has_bbo);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}