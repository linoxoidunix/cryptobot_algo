#include <gtest/gtest.h>

#include <memory>

#include "aos/common/mem_pool.h"
#include "aos/pnl/realized_storage/i_pnl_realized_storage.h"
#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "aos/position_strategy/position_strategy.h"
#include "aos/trading_pair/trading_pair.h"

using Price = double;
using Qty   = double;
// Helper function to initialize the parser manager

// Тестирование NetPositionStrategy для типа double и простой памяти

class NetPositionStrategyTestDefault : public ::testing::Test {
  protected:
    aos::impl::RealizedPnlCalculatorDefault<Price, Qty>
        realized_pnl_calculator_;
    aos::impl::RealizedPnlStorageDefault<Price, Qty> realized_pnl_storage_;
    aos::impl::UnRealizedPnlCalculatorDefault<Price, Qty>
        unrealized_pnl_calculator_;
    aos::impl::NetUnRealizedPnlStorageDefault<Price, Qty>
        un_realized_pnl_storage_;
    aos::impl::NetPositionStrategyDefault<Price, Qty> strategy_;

    void SetUp() override {}
    NetPositionStrategyTestDefault()
        : realized_pnl_storage_(realized_pnl_calculator_),
          un_realized_pnl_storage_(unrealized_pnl_calculator_),
          strategy_(realized_pnl_storage_, un_realized_pnl_storage_) {
        // безопасно инициализировать поля
    }
};

TEST_F(NetPositionStrategyTestDefault,
       AddPositionIncreasesNetQtyAndAveragesPrice) {
    double avg_price = 100.0;
    double net_qty   = 1.0;
    strategy_.Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                  avg_price, net_qty, 130.0, 2.0);

    EXPECT_DOUBLE_EQ(avg_price, 120.0);  // Новая цена должна быть 110.0
    EXPECT_DOUBLE_EQ(net_qty, 3.0);      // Количество позиций должно быть 3.0
}

TEST_F(NetPositionStrategyTestDefault, RemovePositionRealizedPnlForLong) {
    double avg_price = 100.0;
    double net_qty   = 3.0;
    strategy_.Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                     avg_price, net_qty, 120.0, 1.0);
    auto [status, pnl] = realized_pnl_storage_.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 20.0);     // PNL для лонга: (120 - 100) * 1.0 = 20.0
    EXPECT_DOUBLE_EQ(net_qty, 2.0);  // После удаления количество должно быть 2
    EXPECT_DOUBLE_EQ(avg_price, 90.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStrategyTestDefault, RemovePositionRealizedPnlForShort) {
    double avg_price = 120.0;
    double net_qty   = -3.0;
    strategy_.Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                     avg_price, net_qty, 100.0, -1.0);
    auto [status, pnl] = realized_pnl_storage_.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     20.0);  // PNL для шорта: (120 - 100) * 1.0 = 20.0
    EXPECT_DOUBLE_EQ(net_qty,
                     -2.0);  // После удаления количество должно быть -2
    EXPECT_DOUBLE_EQ(avg_price, 130.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStrategyTestDefault, RemoveMoreThanNetQtyForLong) {
    double avg_price = 100.0;
    double net_qty   = 3.0;
    strategy_.Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                     avg_price, net_qty, 120.0, 5.0);
    auto [status, pnl] = realized_pnl_storage_.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     60.0);           // PNL для лонга: (120 - 100) * 3 = 60.0
    EXPECT_DOUBLE_EQ(net_qty, -2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(avg_price, 150.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStrategyTestDefault, RemoveMoreThanNetQtyForShort) {
    double avg_price = 120.0;
    double net_qty   = -3.0;
    strategy_.Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                     avg_price, net_qty, 100.0, -5.0);
    auto [status, pnl] = realized_pnl_storage_.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl,
                     60.0);          // PNL для шорта: (120 - 100) * 3 = 60.0
    EXPECT_DOUBLE_EQ(net_qty, 2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(avg_price, 70.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStrategyTestDefault, RemoveZeroQtyDoesNotChangePosition) {
    double avg_price = 100.0;
    double net_qty   = 3.0;
    strategy_.Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                     avg_price, net_qty, 120.0, 0.0);

    auto [status, pnl] = realized_pnl_storage_.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 0.0);          // Нет изменения в PNL
    EXPECT_DOUBLE_EQ(net_qty, 3.0);      // Количество не изменилось
    EXPECT_DOUBLE_EQ(avg_price, 100.0);  // Средняя цена не изменилась
}

// Testing inralized storage
TEST_F(NetPositionStrategyTestDefault, UnrealizedPnlWithoutBBOIsZero) {
    double avg_price = 100.0;
    double net_qty   = 2.0;

    un_realized_pnl_storage_.UpdatePosition(common::ExchangeId::kBinance,
                                            aos::TradingPair::kBTCUSDT,
                                            avg_price, net_qty);
    auto [status, pnl] = un_realized_pnl_storage_.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);

    EXPECT_FALSE(status);
}

TEST_F(NetPositionStrategyTestDefault, UnrealizedPnlWithBBOAfterPosition) {
    double avg_price = 0;
    double net_qty   = 0;

    // un_realized_pnl_storage_.UpdatePosition(common::ExchangeId::kBinance, {2,
    // 1},
    //                                        avg_price, net_qty);
    strategy_.Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                  avg_price, net_qty, 100.0, 2.0);
    un_realized_pnl_storage_.UpdateBBO(common::ExchangeId::kBinance,
                                       aos::TradingPair::kBTCUSDT, 90.0, 110.0);

    auto [status, pnl] = un_realized_pnl_storage_.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(pnl, -20.0);  // buy 100 sell use bid 90. unrealized
                                   // (90-100)*2 = -20, совпадает с avg => 0
}

TEST_F(NetPositionStrategyTestDefault, UnrealizedPnlWithPositionAfterBBO) {
    double avg_price = 0;
    double net_qty   = 0;

    un_realized_pnl_storage_.UpdateBBO(common::ExchangeId::kBinance,
                                       aos::TradingPair::kBTCUSDT, 90.0, 110.0);
    strategy_.Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                  avg_price, net_qty, 100.0, 2.0);

    auto [status, pnl] = un_realized_pnl_storage_.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(pnl, -20.0);  // (95+105)/2 = 100, (100 - 90) * 2 = 20
}

TEST_F(NetPositionStrategyTestDefault, UnrealizedPnlIsUpdatedOnNewBBO) {
    double avg_price = 0;
    double net_qty   = 0;

    strategy_.Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                  avg_price, net_qty, 100.0, 1.0);
    un_realized_pnl_storage_.UpdateBBO(common::ExchangeId::kBinance,
                                       aos::TradingPair::kBTCUSDT, 90.0, 110.0);

    auto [status1, pnl1] = un_realized_pnl_storage_.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status1);
    EXPECT_DOUBLE_EQ(pnl1, -10.0);  // (-100+90)*2 = -20

    un_realized_pnl_storage_.UpdateBBO(common::ExchangeId::kBinance,
                                       aos::TradingPair::kBTCUSDT, 95.0, 105.0);
    auto [status2, pnl2] = un_realized_pnl_storage_.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status2);
    EXPECT_DOUBLE_EQ(pnl2, -5);  // (95 - 100) * 1
}

TEST_F(NetPositionStrategyTestDefault, GetUnrealizedPnlForNonexistentKey) {
    auto [status, pnl] = un_realized_pnl_storage_.GetUnRealizedPnl(
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