#include <gtest/gtest.h>

#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/order_storage/order_storage.h"  // путь к вашему заголовочному файлу с OrderStorage

using namespace aoe::bybit::impl;
using namespace aoe::bybit;
namespace {

Order MakeOrder(uint64_t id, OrderStatus status, PendingAction pending_action,
                Category category, Side side, OrderMode mode) {
    Order o{};
    o.order_id       = id;
    o.order_status   = status;
    o.pending_action = pending_action;
    o.category       = category;
    o.side           = side;
    o.order_mode     = mode;
    return o;
}

}  // namespace

TEST(OrderStorageTest, AddAndGetById) {
    OrderStorage storage;

    auto order = MakeOrder(1, OrderStatus::kNew, PendingAction::kNone,
                           Category::kLinear, Side::kBuy, OrderMode::kLimit);
    storage.Add(std::move(order));

    auto [found, result] = storage.Get(1);
    ASSERT_TRUE(found);
    EXPECT_EQ(result->order_id, 1);
    EXPECT_EQ(result->order_status, OrderStatus::kNew);
}

TEST(OrderStorageTest, GetByStatus) {
    OrderStorage storage;
    storage.Emplace(Category::kLinear, Side::kBuy, OrderMode::kLimit,
                    OrderStatus::kNew, PendingAction::kNone, 1,
                    aos::TradingPair::kBTCUSDT);
    storage.Emplace(Category::kLinear, Side::kSell, OrderMode::kMarket,
                    OrderStatus::kFilled, PendingAction::kNone, 2,
                    aos::TradingPair::kETHUSDT);
    storage.Emplace(Category::kInverse, Side::kBuy, OrderMode::kLimit,
                    OrderStatus::kNew, PendingAction::kNone, 3,
                    aos::TradingPair::kSOLUSDT);

    auto orders = storage.Get(OrderStatus::kNew);
    ASSERT_EQ(orders.size(), 2);
    EXPECT_EQ(orders[0]->order_status, OrderStatus::kNew);
    EXPECT_EQ(orders[1]->order_status, OrderStatus::kNew);
}

TEST(OrderStorageTest, GetByCategory) {
    OrderStorage storage;
    storage.Emplace(Category::kLinear, Side::kBuy, OrderMode::kLimit,
                    OrderStatus::kNew, PendingAction::kNone, 1,
                    aos::TradingPair::kBTCUSDT);
    storage.Emplace(Category::kInverse, Side::kSell, OrderMode::kMarket,
                    OrderStatus::kNew, PendingAction::kNone, 2,
                    aos::TradingPair::kETHUSDT);
    storage.Emplace(Category::kLinear, Side::kSell, OrderMode::kLimit,
                    OrderStatus::kNew, PendingAction::kNone, 3,
                    aos::TradingPair::kSOLUSDT);

    auto orders = storage.Get(Category::kLinear);
    ASSERT_EQ(orders.size(), 2);
    for (const auto* order : orders)
        EXPECT_EQ(order->category, Category::kLinear);
}

TEST(OrderStorageTest, GetByCategorySide) {
    OrderStorage storage;
    storage.Emplace(Category::kLinear, Side::kBuy, OrderMode::kLimit,
                    OrderStatus::kNew, PendingAction::kNone, 1,
                    aos::TradingPair::kBTCUSDT);
    storage.Emplace(Category::kLinear, Side::kSell, OrderMode::kMarket,
                    OrderStatus::kNew, PendingAction::kNone, 2,
                    aos::TradingPair::kETHUSDT);
    storage.Emplace(Category::kLinear, Side::kBuy, OrderMode::kLimit,
                    OrderStatus::kNew, PendingAction::kNone, 3,
                    aos::TradingPair::kSOLUSDT);

    auto orders = storage.Get(Category::kLinear, Side::kBuy);
    ASSERT_EQ(orders.size(), 2);
    for (const auto* order : orders) {
        EXPECT_EQ(order->category, Category::kLinear);
        EXPECT_EQ(order->side, Side::kBuy);
    }
}

TEST(OrderStorageTest, GetByCategoryMode) {
    OrderStorage storage;
    storage.Emplace(Category::kLinear, Side::kBuy, OrderMode::kLimit,
                    OrderStatus::kNew, PendingAction::kNone, 1,
                    aos::TradingPair::kBTCUSDT);
    storage.Emplace(Category::kLinear, Side::kSell, OrderMode::kMarket,
                    OrderStatus::kNew, PendingAction::kNone, 2,
                    aos::TradingPair::kETHUSDT);
    storage.Emplace(Category::kLinear, Side::kBuy, OrderMode::kLimit,
                    OrderStatus::kNew, PendingAction::kNone, 3,
                    aos::TradingPair::kSOLUSDT);

    auto orders = storage.Get(Category::kLinear, OrderMode::kLimit);
    ASSERT_EQ(orders.size(), 2);
    for (const auto* order : orders) {
        EXPECT_EQ(order->category, Category::kLinear);
        EXPECT_EQ(order->order_mode, OrderMode::kLimit);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}