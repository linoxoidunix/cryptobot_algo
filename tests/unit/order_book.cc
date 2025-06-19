#include "aos/order_book/order_book.h"

#include <gtest/gtest.h>

#include "aoe/aoe.h"  // Ваш класс
#include "aos/aos.h"  // Ваш класс
#include "aos/best_ask/best_ask.h"
#include "aos/best_bid/best_bid.h"
#include "aos/common/mem_pool.h"
#include "aos/order_book_level/order_book_level.h"
#include "gmock/gmock.h"

// using namespace aos;
// using namespace aoe::bybit;
// using namespace aoe::bybit::impl;
// using namespace aos;

// Объявление типа OrderBook с подстановкой
using TestOrderBookInner = aos::OrderBookInner<
    double, double, common::MemoryPoolNotThreadSafety,
    std::unordered_map<double, aos::OrderBookLevel<double, double>*>>;

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

TEST(OrderBookTest, UpdateTopNBidsRejectsIfVectorTooSmall) {
    TestOrderBookInner book{100};
    book.AddBidLevel(100.0, 1.0);
    book.AddBidLevel(99.0, 2.0);
    using Price = double;
    using Qty   = double;
    std::vector<aos::BestBid<Price, Qty>> bids(1);  // меньше чем n
    std::size_t max_lvl = 0;

    bool updated        = book.UpdateTopNBids(2, bids, max_lvl);
    EXPECT_FALSE(updated);
    EXPECT_EQ(max_lvl, 0);
}

TEST(OrderBookTest, UpdateTopNBidsUpdatesCorrectly) {
    TestOrderBookInner book{100};
    book.AddBidLevel(100.0, 1.0);
    book.AddBidLevel(99.0, 2.0);
    book.AddBidLevel(98.0, 3.0);
    using Price = double;
    using Qty   = double;
    std::vector<aos::BestBid<Price, Qty>> bids(3);
    std::size_t max_lvl = 99;  // должен быть перезаписан

    bool updated        = book.UpdateTopNBids(3, bids, max_lvl);
    EXPECT_TRUE(updated);
    EXPECT_EQ(bids[0].bid_price, 100.0);
    EXPECT_EQ(bids[0].bid_qty, 1.0);
    EXPECT_EQ(bids[1].bid_price, 99.0);
    EXPECT_EQ(bids[1].bid_qty, 2.0);
    EXPECT_EQ(bids[2].bid_price, 98.0);
    EXPECT_EQ(bids[2].bid_qty, 3.0);
    EXPECT_EQ(max_lvl, 2);
}

TEST(OrderBookTest, UpdateTopNBidsNoChangesReturnsFalse) {
    TestOrderBookInner book{100};
    book.AddBidLevel(100.0, 1.0);
    book.AddBidLevel(99.0, 2.0);
    using Price                                = double;
    using Qty                                  = double;
    std::vector<aos::BestBid<Price, Qty>> bids = {aos::BestBid{100.0, 1.0},
                                                  aos::BestBid{99.0, 2.0}};
    std::size_t max_lvl = 42;  // должен остаться нетронутым

    bool updated        = book.UpdateTopNBids(2, bids, max_lvl);
    EXPECT_FALSE(updated);
    EXPECT_EQ(max_lvl, 42);
}

TEST(OrderBookTest, UpdateTopNAsksRejectsIfVectorTooSmall) {
    TestOrderBookInner book{100};
    book.AddAskLevel(101.0, 1.0);
    using Price = double;
    using Qty   = double;
    std::vector<aos::BestAsk<Price, Qty>> asks(0);  // пустой
    std::size_t max_lvl = 0;

    bool updated        = book.UpdateTopNAsks(1, asks, max_lvl);
    EXPECT_FALSE(updated);
    EXPECT_EQ(max_lvl, 0);
}

TEST(OrderBookTest, UpdateTopNAsksUpdatesCorrectly) {
    TestOrderBookInner book{100};
    book.AddAskLevel(101.0, 1.0);
    book.AddAskLevel(102.0, 2.0);
    using Price = double;
    using Qty   = double;
    std::vector<aos::BestAsk<Price, Qty>> asks(2);
    std::size_t max_lvl = 999;

    bool updated        = book.UpdateTopNAsks(2, asks, max_lvl);
    EXPECT_TRUE(updated);
    EXPECT_EQ(asks[0].ask_price, 101.0);
    EXPECT_EQ(asks[0].ask_qty, 1.0);
    EXPECT_EQ(asks[1].ask_price, 102.0);
    EXPECT_EQ(asks[1].ask_qty, 2.0);
    EXPECT_EQ(max_lvl, 1);
}

TEST(OrderBookTest, UpdateTopNAsksNoChangesReturnsFalse) {
    TestOrderBookInner book{100};
    book.AddAskLevel(101.0, 1.0);
    book.AddAskLevel(102.0, 2.0);
    using Price                                = double;
    using Qty                                  = double;
    std::vector<aos::BestAsk<Price, Qty>> asks = {aos::BestAsk{101.0, 1.0},
                                                  aos::BestAsk{102.0, 2.0}};
    std::size_t max_lvl                        = 555;

    bool updated = book.UpdateTopNAsks(2, asks, max_lvl);
    EXPECT_FALSE(updated);
    EXPECT_EQ(max_lvl, 555);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}