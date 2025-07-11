#include <gtest/gtest.h>

#include <memory>

#include "aoe/bybit/execution_event/types.h"
#include "aoe/bybit/execution_watcher/execution_watcher.h"
#include "aos/common/mem_pool.h"
#include "aos/pnl/realized_storage/i_pnl_realized_storage.h"
#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "aos/position/position.h"
#include "aos/position_storage/position_storage_by_pair/position_storage_by_pair.h"
#include "aos/position_strategy/position_strategy.h"
#include "aos/trading_pair/trading_pair.h"
using Price = double;
using Qty   = double;
// Helper function to initialize the parser manager

// Тестирование NetPositionStrategy для типа double и простой памяти

class NetPositionStorageDefaultUseExternalEventBybitTest
    : public ::testing::Test {
  protected:
    using PositionT = aos::impl::NetPositionDefault<Price, Qty>;

    aos::impl::RealizedPnlCalculatorDefault<Price, Qty>
        realized_pnl_calculator_;
    aos::impl::RealizedPnlStorageDefault<Price, Qty> realized_pnl_storage_;
    aos::impl::UnRealizedPnlCalculatorDefault<Price, Qty>
        unrealized_pnl_calculator_;
    aos::impl::NetUnRealizedPnlStorageDefault<Price, Qty>
        un_realized_pnl_storage_;
    aos::impl::NetPositionStrategyDefault<Price, Qty> strategy_;
    aos::impl::PositionStorageByPair<Price, Qty,
                                     aos::impl::NetPositionDefault<Price, Qty>>
        position_storage_by_pair_;
    common::MemoryPoolThreadSafety<aoe::bybit::ExecutionEventSpotBuyDefault<
        common::MemoryPoolThreadSafety, PositionT>>
        spot_buy_pool_event_{1};
    common::MemoryPoolThreadSafety<aoe::bybit::ExecutionEventSpotSellDefault<
        common::MemoryPoolThreadSafety, PositionT>>
        spot_sell_pool_event_{1};
    common::MemoryPoolThreadSafety<aoe::bybit::ExecutionEventLinearBuyDefault<
        common::MemoryPoolThreadSafety, PositionT>>
        linear_buy_pool_event_{1};
    common::MemoryPoolThreadSafety<aoe::bybit::ExecutionEventLinearSellDefault<
        common::MemoryPoolThreadSafety, PositionT>>
        linear_sell_pool_event_{1};
    aoe::bybit::impl::ExecutionWatcherDefault<common::MemoryPoolThreadSafety,
                                              PositionT>
        watcher_;
    void SetUp() override {}
    NetPositionStorageDefaultUseExternalEventBybitTest()
        : realized_pnl_storage_(realized_pnl_calculator_),
          un_realized_pnl_storage_(unrealized_pnl_calculator_),
          strategy_(realized_pnl_storage_, un_realized_pnl_storage_),
          position_storage_by_pair_([this]() {
              return aos::impl::NetPositionDefault<Price, Qty>(strategy_);
          }),
          watcher_(position_storage_by_pair_) {
        // безопасно инициализировать поля
    }
};

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemovePositionWithoutAddPostionEarlier) {
    auto* ptr_sell = spot_sell_pool_event_.Allocate();
    ptr_sell->SetMemoryPool(&spot_sell_pool_event_);
    ptr_sell->SetExecPrice(120);
    ptr_sell->SetExecQty(3.0);
    ptr_sell->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_sell);

    auto position = position_storage_by_pair_.GetPosition(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_FALSE(position.has_value());
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       AddPositionIncreasesNetQtyAndAveragesPrice) {
    auto* ptr = spot_buy_pool_event_.Allocate();
    ptr->SetMemoryPool(&spot_buy_pool_event_);
    ptr->SetExecPrice(120);
    ptr->SetExecQty(3.0);
    ptr->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr);

    auto position = position_storage_by_pair_.GetPosition(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     120.0);  // Новая цена должна быть 110.0
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     3.0);  // Количество позиций должно быть 3.0
    auto position_binance = position_storage_by_pair_.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_FALSE(position_binance.has_value());
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemovePositionRealizedPnlForLong) {
    auto* ptr_buy = spot_buy_pool_event_.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event_);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(3.0);
    ptr_buy->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_buy);

    auto* ptr_sell = spot_sell_pool_event_.Allocate();
    ptr_sell->SetMemoryPool(&spot_sell_pool_event_);
    ptr_sell->SetExecPrice(120);
    ptr_sell->SetExecQty(1.0);
    ptr_sell->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_sell);

    auto [status, pnl] = realized_pnl_storage_.GetRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 20.0);  // PNL для лонга: (120 - 100) * 1.0 = 20.0
    auto position = position_storage_by_pair_.GetPosition(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     2.0);  // После удаления количество должно быть 2
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     90.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemovePositionRealizedPnlForShort) {
    auto* ptr_sell_linear = linear_sell_pool_event_.Allocate();
    ptr_sell_linear->SetMemoryPool(&linear_sell_pool_event_);
    ptr_sell_linear->SetExecPrice(120);
    ptr_sell_linear->SetExecQty(3.0);
    ptr_sell_linear->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_sell_linear);

    auto* ptr_buy_linear = linear_buy_pool_event_.Allocate();
    ptr_buy_linear->SetMemoryPool(&linear_buy_pool_event_);
    ptr_buy_linear->SetExecPrice(100);
    ptr_buy_linear->SetExecQty(1.0);
    ptr_buy_linear->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_buy_linear);

    auto [status, pnl] = realized_pnl_storage_.GetRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     20.0);  // PNL для шорта: (120 - 100) * 1.0 = 20.0
    auto position = position_storage_by_pair_.GetPosition(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     -2.0);  // После удаления количество должно быть -2
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     130.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemoveMoreThanNetQtyForLong) {
    auto* ptr_buy = spot_buy_pool_event_.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event_);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(3.0);
    ptr_buy->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_buy);

    auto* ptr_sell = spot_sell_pool_event_.Allocate();
    ptr_sell->SetMemoryPool(&spot_sell_pool_event_);
    ptr_sell->SetExecPrice(120);
    ptr_sell->SetExecQty(5.0);
    ptr_sell->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_sell);

    auto [status, pnl] = realized_pnl_storage_.GetRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     60.0);  // PNL для лонга: (120 - 100) * 3 = 60.0
    auto position = position_storage_by_pair_.GetPosition(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     -2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     150.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemoveMoreThanNetQtyForShort) {
    auto* ptr_sell_linear = linear_sell_pool_event_.Allocate();
    ptr_sell_linear->SetMemoryPool(&linear_sell_pool_event_);
    ptr_sell_linear->SetExecPrice(120);
    ptr_sell_linear->SetExecQty(3.0);
    ptr_sell_linear->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_sell_linear);

    auto* ptr_buy_linear = linear_buy_pool_event_.Allocate();
    ptr_buy_linear->SetMemoryPool(&linear_buy_pool_event_);
    ptr_buy_linear->SetExecPrice(100);
    ptr_buy_linear->SetExecQty(5.0);
    ptr_buy_linear->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_buy_linear);

    auto [status, pnl] = realized_pnl_storage_.GetRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl,
                     60.0);  // PNL для шорта: (120 - 100) * 3 = 60.0
    auto position = position_storage_by_pair_.GetPosition(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     70.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemoveZeroQtyDoesNotChangePosition) {
    auto* ptr_buy = spot_buy_pool_event_.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event_);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(3.0);
    ptr_buy->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_buy);

    auto* ptr_sell = spot_sell_pool_event_.Allocate();
    ptr_sell->SetMemoryPool(&spot_sell_pool_event_);
    ptr_sell->SetExecPrice(120);
    ptr_sell->SetExecQty(0.0);
    ptr_sell->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_sell);

    auto [status, pnl] = realized_pnl_storage_.GetRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 0.0);  // Нет изменения в PNL
    auto position = position_storage_by_pair_.GetPosition(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     3.0);  // Количество не изменилось
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     100.0);  // Средняя цена не изменилась
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       UnrealizedPnlWithBBOAfterPosition) {
    auto* ptr_buy = spot_buy_pool_event_.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event_);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(2.0);
    ptr_buy->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_buy);

    un_realized_pnl_storage_.UpdateBBO(common::ExchangeId::kBybit,
                                       aos::TradingPair::kBTCUSDT, 90.0, 110.0);

    auto [status, pnl] = un_realized_pnl_storage_.GetUnRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(pnl, -20.0);  // buy 100 sell use bid 90. unrealized
                                   // (90-100)*2 = -20, совпадает с avg => 0
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       UnrealizedPnlWithPositionAfterBBO) {
    un_realized_pnl_storage_.UpdateBBO(common::ExchangeId::kBybit,
                                       aos::TradingPair::kBTCUSDT, 90.0, 110.0);
    auto* ptr_buy = spot_buy_pool_event_.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event_);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(2.0);
    ptr_buy->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_buy);

    auto [status, pnl] = un_realized_pnl_storage_.GetUnRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(pnl, -20.0);  // (95+105)/2 = 100, (100 - 90) * 2 = 20
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       UnrealizedPnlIsUpdatedOnNewBBO) {
    auto* ptr_buy = spot_buy_pool_event_.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event_);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(1.0);
    ptr_buy->SetTradingPair(aos::TradingPair::kBTCUSDT);
    watcher_.OnEvent(ptr_buy);

    un_realized_pnl_storage_.UpdateBBO(common::ExchangeId::kBybit,
                                       aos::TradingPair::kBTCUSDT, 90.0, 110.0);

    auto [status1, pnl1] = un_realized_pnl_storage_.GetUnRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status1);
    EXPECT_DOUBLE_EQ(pnl1, -10.0);  // (-100+90)*2 = -20

    un_realized_pnl_storage_.UpdateBBO(common::ExchangeId::kBybit,
                                       aos::TradingPair::kBTCUSDT, 95.0, 105.0);
    auto [status2, pnl2] = un_realized_pnl_storage_.GetUnRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status2);
    EXPECT_DOUBLE_EQ(pnl2, -5);  // (95 - 100) * 1
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       GetUnrealizedPnlForNonexistentKey) {
    auto [status, pnl] = un_realized_pnl_storage_.GetUnRealizedPnl(
        common::ExchangeId::kBybit, aos::TradingPair::kCount);
    EXPECT_FALSE(status);
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}