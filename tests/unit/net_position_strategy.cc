#include <gtest/gtest.h>

#include <memory>

#include "aos/pnl/realized_storage/i_pnl_realized_storage.h"
#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "aos/position_strategy/position_strategy.h"
#include "aos/trading_pair/trading_pair.h"
#include "aos/common/mem_pool.h"

using Price = double;
using Qty   = double;
// Helper function to initialize the parser manager

// Тестирование NetPositionStrategy для типа double и простой памяти

class NetPositionStrategyTest : public ::testing::Test {
  protected:
    using RealizedPnlStorageT =
        aos::impl::RealizedPnlStorage<Price, Qty,
                                      common::MemoryPoolNotThreadSafety>;
    aos::impl::RealizedPnlStorageContainer<
        Price, Qty, common::MemoryPoolNotThreadSafety, RealizedPnlStorageT>
        realized_container{1};
    using UnRealizedPnlCalculatorContainerT =
        aos::impl::UnRealizedPnlCalculatorContainer<
            Price, Qty, common::MemoryPoolNotThreadSafety,
            aos::impl::UnRealizedPnlCalculator<
                Price, Qty, common::MemoryPoolNotThreadSafety>>;
    using UnrealizedPnlStorageContainerT =
        aos::impl::UnRealizedPnlStorageContainer<
            Price, Qty, common::MemoryPoolNotThreadSafety,
            UnRealizedPnlCalculatorContainerT,
            aos::impl::NetUnRealizedPnlStorage<
                Price, Qty, common::MemoryPoolNotThreadSafety>>;
    UnrealizedPnlStorageContainerT unrealized_container{1};
    boost::intrusive_ptr<
        aos::IPnlRealizedStorage<Price, Qty, common::MemoryPoolNotThreadSafety>>
        ptr_realized_storage;
    boost::intrusive_ptr<aos::IPnlUnRealizedStorage<
        Price, Qty, common::MemoryPoolNotThreadSafety>>
        ptr_unrealized_storage;

    std::unique_ptr<aos::impl::NetPositionStrategy<
        Price, Qty, common::MemoryPoolNotThreadSafety>>
        strategy;

    void SetUp() override {
        ptr_realized_storage   = realized_container.Build();
        ptr_unrealized_storage = unrealized_container.Build();
        strategy = std::make_unique<aos::impl::NetPositionStrategy<
            Price, Qty, common::MemoryPoolNotThreadSafety>>(
            ptr_realized_storage, ptr_unrealized_storage);
    }
};

TEST_F(NetPositionStrategyTest, AddPositionIncreasesNetQtyAndAveragesPrice) {
    double avg_price = 100.0;
    double net_qty   = 1.0;
    strategy->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                  avg_price, net_qty, 130.0, 2.0);

    EXPECT_DOUBLE_EQ(avg_price, 120.0);  // Новая цена должна быть 110.0
    EXPECT_DOUBLE_EQ(net_qty, 3.0);      // Количество позиций должно быть 3.0
}

TEST_F(NetPositionStrategyTest, RemovePositionRealizedPnlForLong) {
    double avg_price = 100.0;
    double net_qty   = 3.0;
    strategy->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                     avg_price, net_qty, 120.0, 1.0);
    auto [status, pnl] = ptr_realized_storage->GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 20.0);     // PNL для лонга: (120 - 100) * 1.0 = 20.0
    EXPECT_DOUBLE_EQ(net_qty, 2.0);  // После удаления количество должно быть 2
    EXPECT_DOUBLE_EQ(avg_price, 90.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStrategyTest, RemovePositionRealizedPnlForShort) {
    double avg_price = 120.0;
    double net_qty   = -3.0;
    strategy->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                     avg_price, net_qty, 100.0, -1.0);
    auto [status, pnl] = ptr_realized_storage->GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
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
    strategy->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                     avg_price, net_qty, 120.0, 5.0);
    auto [status, pnl] = ptr_realized_storage->GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     60.0);           // PNL для лонга: (120 - 100) * 3 = 60.0
    EXPECT_DOUBLE_EQ(net_qty, -2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(avg_price, 150.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStrategyTest, RemoveMoreThanNetQtyForShort) {
    double avg_price = 120.0;
    double net_qty   = -3.0;
    strategy->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                     avg_price, net_qty, 100.0, -5.0);
    auto [status, pnl] = ptr_realized_storage->GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl,
                     60.0);          // PNL для шорта: (120 - 100) * 3 = 60.0
    EXPECT_DOUBLE_EQ(net_qty, 2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(avg_price, 70.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStrategyTest, RemoveZeroQtyDoesNotChangePosition) {
    double avg_price = 100.0;
    double net_qty   = 3.0;
    strategy->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                     avg_price, net_qty, 120.0, 0.0);

    auto [status, pnl] = ptr_realized_storage->GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 0.0);          // Нет изменения в PNL
    EXPECT_DOUBLE_EQ(net_qty, 3.0);      // Количество не изменилось
    EXPECT_DOUBLE_EQ(avg_price, 100.0);  // Средняя цена не изменилась
}

// Testing inralized storage
TEST_F(NetPositionStrategyTest, UnrealizedPnlWithoutBBOIsZero) {
    double avg_price = 100.0;
    double net_qty   = 2.0;

    ptr_unrealized_storage->UpdatePosition(common::ExchangeId::kBinance,
                                           aos::TradingPair::kBTCUSDT,
                                           avg_price, net_qty);
    auto [status, pnl] = ptr_unrealized_storage->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);

    EXPECT_FALSE(status);
}

TEST_F(NetPositionStrategyTest, UnrealizedPnlWithBBOAfterPosition) {
    double avg_price = 0;
    double net_qty   = 0;

    // ptr_unrealized_storage->UpdatePosition(common::ExchangeId::kBinance, {2,
    // 1},
    //                                        avg_price, net_qty);
    strategy->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                  avg_price, net_qty, 100.0, 2.0);
    ptr_unrealized_storage->UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 90.0, 110.0);

    auto [status, pnl] = ptr_unrealized_storage->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(pnl, -20.0);  // buy 100 sell use bid 90. unrealized
                                   // (90-100)*2 = -20, совпадает с avg => 0
}

TEST_F(NetPositionStrategyTest, UnrealizedPnlWithPositionAfterBBO) {
    double avg_price = 0;
    double net_qty   = 0;

    ptr_unrealized_storage->UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 90.0, 110.0);
    strategy->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                  avg_price, net_qty, 100.0, 2.0);

    auto [status, pnl] = ptr_unrealized_storage->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(pnl, -20.0);  // (95+105)/2 = 100, (100 - 90) * 2 = 20
}

TEST_F(NetPositionStrategyTest, UnrealizedPnlIsUpdatedOnNewBBO) {
    double avg_price = 0;
    double net_qty   = 0;

    strategy->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                  avg_price, net_qty, 100.0, 1.0);
    ptr_unrealized_storage->UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 90.0, 110.0);

    auto [status1, pnl1] = ptr_unrealized_storage->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status1);
    EXPECT_DOUBLE_EQ(pnl1, -10.0);  // (-100+90)*2 = -20

    ptr_unrealized_storage->UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 95.0, 105.0);
    auto [status2, pnl2] = ptr_unrealized_storage->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status2);
    EXPECT_DOUBLE_EQ(pnl2, -5);  // (95 - 100) * 1
}

TEST_F(NetPositionStrategyTest, GetUnrealizedPnlForNonexistentKey) {
    auto [status, pnl] = ptr_unrealized_storage->GetUnRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kCount);
    EXPECT_FALSE(status);
    EXPECT_DOUBLE_EQ(pnl, 0.0);
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}