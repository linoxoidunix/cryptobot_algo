#include <gtest/gtest.h>

#include <memory>

#include "aos/pnl/realized_storage/i_pnl_realized_storage.h"
#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "aos/position/position.h"
#include "aos/position_storage/position_storage_by_pair/position_storage_by_pair.h"
#include "aos/position_strategy/position_strategy.h"
#include "aos/trading_pair/trading_pair.h"
#include "aot/common/mem_pool.h"

using Price = double;
using Qty   = double;
// Helper function to initialize the parser manager

// Тестирование NetPositionStrategy для типа double и простой памяти

class NetPositionStorageDefaultTest : public ::testing::Test {
  protected:
    aos::impl::RealizedPnlCalculatorDefault<Price, Qty> realized_pnl_calculator;
    aos::impl::RealizedPnlStorageDefault<Price, Qty> realized_pnl_storage;
    aos::impl::UnRealizedPnlCalculatorDefault<Price, Qty>
        unrealized_pnl_calculator;
    aos::impl::NetUnRealizedPnlStorageDefault<Price, Qty>
        un_realized_pnl_storage;
    aos::impl::NetPositionStrategyDefault<Price, Qty> strategy;
    aos::impl::PositionStorageByPair<Price, Qty,
                                     aos::impl::NetPositionDefault<Price, Qty>>
        position_storage_by_pair;
    void SetUp() override {}
    NetPositionStorageDefaultTest()
        : realized_pnl_storage(realized_pnl_calculator),
          un_realized_pnl_storage(unrealized_pnl_calculator),
          strategy(realized_pnl_storage, un_realized_pnl_storage),
          position_storage_by_pair([this]() {
              return aos::impl::NetPositionDefault<Price, Qty>(strategy);
          }) {
        // безопасно инициализировать поля
    }
};

TEST_F(NetPositionStorageDefaultTest, RemovePositionWithoutAddPostionEarlier) {
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, 3.0);
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_FALSE(position.has_value());
}

TEST_F(NetPositionStorageDefaultTest,
       AddPositionIncreasesNetQtyAndAveragesPrice) {
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, 3.0);
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     120.0);  // Новая цена должна быть 110.0
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     3.0);  // Количество позиций должно быть 3.0
}

TEST_F(NetPositionStorageDefaultTest, RemovePositionRealizedPnlForLong) {
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 3.0);
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, 1.0);
    auto [status, pnl] = realized_pnl_storage.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 20.0);  // PNL для лонга: (120 - 100) * 1.0 = 20.0
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     2.0);  // После удаления количество должно быть 2
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     90.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStorageDefaultTest, RemovePositionRealizedPnlForShort) {
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, -3.0);
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, -1.0);
    auto [status, pnl] = realized_pnl_storage.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     20.0);  // PNL для шорта: (120 - 100) * 1.0 = 20.0
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     -2.0);  // После удаления количество должно быть -2
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     130.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStorageDefaultTest, RemoveMoreThanNetQtyForLong) {
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 3.0);
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, 5.0);
    auto [status, pnl] = realized_pnl_storage.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     60.0);  // PNL для лонга: (120 - 100) * 3 = 60.0
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     -2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     150.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStorageDefaultTest, RemoveMoreThanNetQtyForShort) {
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, -3.0);
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, -5.0);
    auto [status, pnl] = realized_pnl_storage.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl,
                     60.0);  // PNL для шорта: (120 - 100) * 3 = 60.0
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     70.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStorageDefaultTest, RemoveZeroQtyDoesNotChangePosition) {
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 3.0);
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, 0.0);

    auto [status, pnl] = realized_pnl_storage.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 0.0);  // Нет изменения в PNL
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     3.0);  // Количество не изменилось
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     100.0);  // Средняя цена не изменилась
}

TEST_F(NetPositionStorageDefaultTest, UnrealizedPnlWithBBOAfterPosition) {
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 2.0);
    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 90.0, 110.0);

    auto [status, pnl] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(pnl, -20.0);  // buy 100 sell use bid 90. unrealized
                                   // (90-100)*2 = -20, совпадает с avg => 0
}

TEST_F(NetPositionStorageDefaultTest, UnrealizedPnlWithPositionAfterBBO) {
    double avg_price = 0;
    double net_qty   = 0;

    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 90.0, 110.0);
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 2.0);

    auto [status, pnl] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(pnl, -20.0);  // (95+105)/2 = 100, (100 - 90) * 2 = 20
}

TEST_F(NetPositionStorageDefaultTest, UnrealizedPnlIsUpdatedOnNewBBO) {
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 1.0);
    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 90.0, 110.0);

    auto [status1, pnl1] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status1);
    EXPECT_DOUBLE_EQ(pnl1, -10.0);  // (-100+90)*2 = -20

    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 95.0, 105.0);
    auto [status2, pnl2] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status2);
    EXPECT_DOUBLE_EQ(pnl2, -5);  // (95 - 100) * 1
}

TEST_F(NetPositionStorageDefaultTest, GetUnrealizedPnlForNonexistentKey) {
    auto [status, pnl] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kCount);
    EXPECT_FALSE(status);
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}