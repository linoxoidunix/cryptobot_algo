#include <gtest/gtest.h>

#include <memory>

#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/position_strategy/position_strategy.h"
#include "aot/common/mem_pool.h"

using Price = double;
using Qty   = double;
// Helper function to initialize the parser manager

// Тестирование NetPositionStrategy для типа double и простой памяти

class NetPositionStrategyTest : public ::testing::Test {
  protected:
    aos::impl::RealizedPnlStorageContainer<Price, Qty,
                                           common::MemoryPoolNotThreadSafety>
        realized_pnl{1};
    boost::intrusive_ptr<aos::impl::RealizedPnlStorage<
        Price, Qty, common::MemoryPoolNotThreadSafety>>
        ptr;
    std::unique_ptr<aos::impl::NetPositionStrategy<
        Price, Qty, common::MemoryPoolNotThreadSafety>>
        strategy;

    void SetUp() override {
        ptr      = realized_pnl.Build();
        strategy = std::make_unique<aos::impl::NetPositionStrategy<
            Price, Qty, common::MemoryPoolNotThreadSafety>>(ptr);
    }
};

TEST_F(NetPositionStrategyTest, AddPositionIncreasesNetQtyAndAveragesPrice) {
    double avg_price = 100.0;
    double net_qty   = 1.0;
    strategy->Add(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                  130.0, 2.0);

    EXPECT_DOUBLE_EQ(avg_price, 120.0);  // Новая цена должна быть 110.0
    EXPECT_DOUBLE_EQ(net_qty, 3.0);      // Количество позиций должно быть 3.0
}

TEST_F(NetPositionStrategyTest, RemovePositionRealizedPnlForLong) {
    double avg_price = 100.0;
    double net_qty   = 3.0;
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     120.0, 1.0);
    auto [status, pnl] =
        ptr->GetRealizedPnl(common::ExchangeId::kBinance, {2, 1});
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 20.0);     // PNL для лонга: (120 - 100) * 1.0 = 20.0
    EXPECT_DOUBLE_EQ(net_qty, 2.0);  // После удаления количество должно быть 2
    EXPECT_DOUBLE_EQ(avg_price, 90.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStrategyTest, RemovePositionRealizedPnlForShort) {
    double avg_price = 120.0;
    double net_qty   = -3.0;
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     100.0, -1.0);
    auto [status, pnl] =
        ptr->GetRealizedPnl(common::ExchangeId::kBinance, {2, 1});
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     20.0);  // PNL для шорта: (120 - 100) * 1.0 = 20.0
    EXPECT_DOUBLE_EQ(net_qty,
                     -2.0);  // После удаления количество должно быть -2
    EXPECT_DOUBLE_EQ(avg_price, 130.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStrategyTest, RemoveMoreThanNetQtyForLong) {
    double avg_price = 100.0;
    double net_qty   = 3.0;
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     120.0, 5.0);
    auto [status, pnl] =
        ptr->GetRealizedPnl(common::ExchangeId::kBinance, {2, 1});
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     60.0);           // PNL для лонга: (120 - 100) * 3 = 60.0
    EXPECT_DOUBLE_EQ(net_qty, -2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(avg_price, 150.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStrategyTest, RemoveMoreThanNetQtyForShort) {
    double avg_price = 120.0;
    double net_qty   = -3.0;
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     100.0, -5.0);
    auto [status, pnl] =
        ptr->GetRealizedPnl(common::ExchangeId::kBinance, {2, 1});
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl,
                     60.0);          // PNL для шорта: (120 - 100) * 3 = 60.0
    EXPECT_DOUBLE_EQ(net_qty, 2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(avg_price, 70.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStrategyTest, RemoveZeroQtyDoesNotChangePosition) {
    double avg_price = 100.0;
    double net_qty   = 3.0;
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     120.0, 0.0);

    auto [status, pnl] =
        ptr->GetRealizedPnl(common::ExchangeId::kBinance, {2, 1});
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 0.0);          // Нет изменения в PNL
    EXPECT_DOUBLE_EQ(net_qty, 3.0);      // Количество не изменилось
    EXPECT_DOUBLE_EQ(avg_price, 100.0);  // Средняя цена не изменилась
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}