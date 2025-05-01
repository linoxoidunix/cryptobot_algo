#include <gtest/gtest.h>

#include <memory>

#include "aoe/bybit/execution_event/types.h"
#include "aoe/bybit/execution_watcher/execution_watcher.h"
#include "aos/pnl/realized_storage/i_pnl_realized_storage.h"
#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "aos/position/position.h"
#include "aos/position_storage/position_storage_by_pair/position_storage_by_pair.h"
#include "aos/position_strategy/position_strategy.h"
#include "aot/common/mem_pool.h"
using Price = double;
using Qty   = double;
// Helper function to initialize the parser manager

// Тестирование NetPositionStrategy для типа double и простой памяти

class NetPositionStorageDefaultUseExternalEventBybitTest
    : public ::testing::Test {
  protected:
    using PositionT = aos::impl::NetPositionDefault<Price, Qty>;

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
    common::MemoryPoolThreadSafety<aoe::bybit::ExecutionEventSpotBuyDefault<
        common::MemoryPoolThreadSafety, PositionT>>
        spot_buy_pool_event{1};
    common::MemoryPoolThreadSafety<aoe::bybit::ExecutionEventSpotSellDefault<
        common::MemoryPoolThreadSafety, PositionT>>
        spot_sell_pool_event{1};
    common::MemoryPoolThreadSafety<aoe::bybit::ExecutionEventLinearBuyDefault<
        common::MemoryPoolThreadSafety, PositionT>>
        linear_buy_pool_event{1};
    common::MemoryPoolThreadSafety<aoe::bybit::ExecutionEventLinearSellDefault<
        common::MemoryPoolThreadSafety, PositionT>>
        linear_sell_pool_event{1};
    aoe::bybit::ExecutionWatcherDefault<common::MemoryPoolThreadSafety,
                                        PositionT>
        watcher;
    void SetUp() override {}
    NetPositionStorageDefaultUseExternalEventBybitTest()
        : realized_pnl_storage(realized_pnl_calculator),
          un_realized_pnl_storage(unrealized_pnl_calculator),
          strategy(realized_pnl_storage, un_realized_pnl_storage),
          position_storage_by_pair([this]() {
              return aos::impl::NetPositionDefault<Price, Qty>(strategy);
          }),
          watcher(position_storage_by_pair) {
        // безопасно инициализировать поля
    }
};

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemovePositionWithoutAddPostionEarlier) {
    auto ptr_sell = spot_sell_pool_event.Allocate();
    ptr_sell->SetMemoryPool(&spot_sell_pool_event);
    ptr_sell->SetExecPrice(120);
    ptr_sell->SetExecQty(3.0);
    ptr_sell->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_sell);

    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_FALSE(position.has_value());
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       AddPositionIncreasesNetQtyAndAveragesPrice) {
    auto ptr = spot_buy_pool_event.Allocate();
    ptr->SetMemoryPool(&spot_buy_pool_event);
    ptr->SetExecPrice(120);
    ptr->SetExecQty(3.0);
    ptr->SetTradingPair({2, 1});
    watcher.OnEvent(ptr);

    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     120.0);  // Новая цена должна быть 110.0
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     3.0);  // Количество позиций должно быть 3.0
    auto position_binance = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, {2, 1});
    EXPECT_FALSE(position_binance.has_value());
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemovePositionRealizedPnlForLong) {
    auto ptr_buy = spot_buy_pool_event.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(3.0);
    ptr_buy->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_buy);

    auto ptr_sell = spot_sell_pool_event.Allocate();
    ptr_sell->SetMemoryPool(&spot_sell_pool_event);
    ptr_sell->SetExecPrice(120);
    ptr_sell->SetExecQty(1.0);
    ptr_sell->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_sell);

    auto [status, pnl] =
        realized_pnl_storage.GetRealizedPnl(common::ExchangeId::kBybit, {2, 1});
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 20.0);  // PNL для лонга: (120 - 100) * 1.0 = 20.0
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     2.0);  // После удаления количество должно быть 2
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     90.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemovePositionRealizedPnlForShort) {
    auto ptr_sell_linear = linear_sell_pool_event.Allocate();
    ptr_sell_linear->SetMemoryPool(&linear_sell_pool_event);
    ptr_sell_linear->SetExecPrice(120);
    ptr_sell_linear->SetExecQty(3.0);
    ptr_sell_linear->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_sell_linear);

    auto ptr_buy_linear = linear_buy_pool_event.Allocate();
    ptr_buy_linear->SetMemoryPool(&linear_buy_pool_event);
    ptr_buy_linear->SetExecPrice(100);
    ptr_buy_linear->SetExecQty(1.0);
    ptr_buy_linear->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_buy_linear);

    auto [status, pnl] =
        realized_pnl_storage.GetRealizedPnl(common::ExchangeId::kBybit, {2, 1});
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     20.0);  // PNL для шорта: (120 - 100) * 1.0 = 20.0
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     -2.0);  // После удаления количество должно быть -2
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     130.0);  // Средняя цена остается 110.0
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemoveMoreThanNetQtyForLong) {
    auto ptr_buy = spot_buy_pool_event.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(3.0);
    ptr_buy->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_buy);

    auto ptr_sell = spot_sell_pool_event.Allocate();
    ptr_sell->SetMemoryPool(&spot_sell_pool_event);
    ptr_sell->SetExecPrice(120);
    ptr_sell->SetExecQty(5.0);
    ptr_sell->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_sell);

    auto [status, pnl] =
        realized_pnl_storage.GetRealizedPnl(common::ExchangeId::kBybit, {2, 1});
    EXPECT_EQ(status, true);

    EXPECT_DOUBLE_EQ(pnl,
                     60.0);  // PNL для лонга: (120 - 100) * 3 = 60.0
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     -2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     150.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemoveMoreThanNetQtyForShort) {
    auto ptr_sell_linear = linear_sell_pool_event.Allocate();
    ptr_sell_linear->SetMemoryPool(&linear_sell_pool_event);
    ptr_sell_linear->SetExecPrice(120);
    ptr_sell_linear->SetExecQty(3.0);
    ptr_sell_linear->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_sell_linear);

    auto ptr_buy_linear = linear_buy_pool_event.Allocate();
    ptr_buy_linear->SetMemoryPool(&linear_buy_pool_event);
    ptr_buy_linear->SetExecPrice(100);
    ptr_buy_linear->SetExecQty(5.0);
    ptr_buy_linear->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_buy_linear);

    auto [status, pnl] =
        realized_pnl_storage.GetRealizedPnl(common::ExchangeId::kBybit, {2, 1});
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl,
                     60.0);  // PNL для шорта: (120 - 100) * 3 = 60.0
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     2.0);  // После удаления количество должно быть 0
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     70.0);  // Средняя цена должна быть сброшена
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       RemoveZeroQtyDoesNotChangePosition) {
    auto ptr_buy = spot_buy_pool_event.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(3.0);
    ptr_buy->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_buy);

    auto ptr_sell = spot_sell_pool_event.Allocate();
    ptr_sell->SetMemoryPool(&spot_sell_pool_event);
    ptr_sell->SetExecPrice(120);
    ptr_sell->SetExecQty(0.0);
    ptr_sell->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_sell);

    auto [status, pnl] =
        realized_pnl_storage.GetRealizedPnl(common::ExchangeId::kBybit, {2, 1});
    EXPECT_EQ(status, true);
    EXPECT_DOUBLE_EQ(pnl, 0.0);  // Нет изменения в PNL
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_TRUE(position.has_value());
    EXPECT_DOUBLE_EQ(position.value().get().GetPosition(),
                     3.0);  // Количество не изменилось
    EXPECT_DOUBLE_EQ(position.value().get().GetAveragePrice(),
                     100.0);  // Средняя цена не изменилась
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       UnrealizedPnlWithBBOAfterPosition) {
    auto ptr_buy = spot_buy_pool_event.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(2.0);
    ptr_buy->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_buy);

    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBybit, {2, 1}, 90.0,
                                      110.0);

    auto [status, pnl] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(pnl, -20.0);  // buy 100 sell use bid 90. unrealized
                                   // (90-100)*2 = -20, совпадает с avg => 0
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       UnrealizedPnlWithPositionAfterBBO) {
    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBybit, {2, 1}, 90.0,
                                      110.0);
    auto ptr_buy = spot_buy_pool_event.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(2.0);
    ptr_buy->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_buy);

    auto [status, pnl] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(pnl, -20.0);  // (95+105)/2 = 100, (100 - 90) * 2 = 20
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       UnrealizedPnlIsUpdatedOnNewBBO) {
    auto ptr_buy = spot_buy_pool_event.Allocate();
    ptr_buy->SetMemoryPool(&spot_buy_pool_event);
    ptr_buy->SetExecPrice(100);
    ptr_buy->SetExecQty(1.0);
    ptr_buy->SetTradingPair({2, 1});
    watcher.OnEvent(ptr_buy);

    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBybit, {2, 1}, 90.0,
                                      110.0);

    auto [status1, pnl1] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_TRUE(status1);
    EXPECT_DOUBLE_EQ(pnl1, -10.0);  // (-100+90)*2 = -20

    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBybit, {2, 1}, 95.0,
                                      105.0);
    auto [status2, pnl2] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBybit, {2, 1});
    EXPECT_TRUE(status2);
    EXPECT_DOUBLE_EQ(pnl2, -5);  // (95 - 100) * 1
}

TEST_F(NetPositionStorageDefaultUseExternalEventBybitTest,
       GetUnrealizedPnlForNonexistentKey) {
    auto [status, pnl] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBybit, {1, 2});
    EXPECT_FALSE(status);
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}