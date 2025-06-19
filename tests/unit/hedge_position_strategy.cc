#include <gtest/gtest.h>

#include <memory>

#include "aos/common/mem_pool.h"
#include "aos/pnl/realized_storage/i_pnl_realized_storage.h"
#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "aos/position_strategy/position_strategy.h"
#include "aos/trading_pair/trading_pair.h"
// Helper function to initialize the parser manager

using Price = double;
using Qty   = double;

class HedgedPositionStrategyTest : public ::testing::Test {
  protected:
    using RealizedPnlStorageT =
        aos::impl::RealizedPnlStorage<Price, Qty,
                                      common::MemoryPoolNotThreadSafety>;
    aos::impl::RealizedPnlStorageContainer<
        Price, Qty, common::MemoryPoolNotThreadSafety, RealizedPnlStorageT>
        realized_container_{1};
    using UnRealizedPnlCalculatorContainerT =
        aos::impl::UnRealizedPnlCalculatorContainer<
            Price, Qty, common::MemoryPoolNotThreadSafety,
            aos::impl::UnRealizedPnlCalculator<
                Price, Qty, common::MemoryPoolNotThreadSafety>>;
    using UnrealizedPnlStorageContainerT =
        aos::impl::UnRealizedPnlStorageContainer<
            Price, Qty, common::MemoryPoolNotThreadSafety,
            UnRealizedPnlCalculatorContainerT,
            aos::impl::HedgedUnRealizedPnlStorage<
                Price, Qty, common::MemoryPoolNotThreadSafety>>;
    UnrealizedPnlStorageContainerT unrealized_container_{1};

    boost::intrusive_ptr<
        aos::IPnlRealizedStorage<Price, Qty, common::MemoryPoolNotThreadSafety>>
        ptr_realized_storage_;
    boost::intrusive_ptr<aos::IPnlUnRealizedStorage<
        Price, Qty, common::MemoryPoolNotThreadSafety>>
        ptr_unrealized_storage_;

    std::unique_ptr<aos::impl::HedgedPositionStrategy<
        Price, Qty, common::MemoryPoolNotThreadSafety>>
        strategy_;

    void SetUp() override {
        ptr_realized_storage_   = realized_container_.Build();
        ptr_unrealized_storage_ = unrealized_container_.Build();
        strategy_ = std::make_unique<aos::impl::HedgedPositionStrategy<
            Price, Qty, common::MemoryPoolNotThreadSafety>>(
            ptr_realized_storage_, ptr_unrealized_storage_);
    }
};

TEST_F(HedgedPositionStrategyTest, AddAndRemoveTest) {
    using Price        = double;
    using Qty          = double;

    // Инициализация переменных
    Price avg_price[2] = {0, 0};  // Средняя цена для long (1) и short (0)
    Qty net_qty[2]     = {0, 0};  // Количество для long и short

    // Добавляем позицию с положительной ценой (long)
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 50.0, 100);
    EXPECT_EQ(net_qty[1], 100);  // Количество в long должно быть 100
    EXPECT_DOUBLE_EQ(avg_price[1],
                     50.0);  // Средняя цена в long должна быть 50.0

    // Добавляем ещё 50 единиц с более высокой ценой (long)
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 60.0, 50);
    EXPECT_EQ(net_qty[1], 150);  // Количество в long должно быть 150
    EXPECT_DOUBLE_EQ(avg_price[1],
                     53.333333333333333333);  // Средняя цена должна быть
                                              // (50*100 + 60*50) / 150
                                              // = 53.33333333333333333

    // Добавляем позицию с отрицательной ценой (short)
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 60.0, -50);
    EXPECT_EQ(net_qty[0], -50);  // Количество в short должно быть 50
    EXPECT_DOUBLE_EQ(avg_price[0],
                     60.0);  // Средняя цена в short должна быть 60.0

    // Удаляем 50 единиц по цене 55 (для long)
    strategy_->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                      avg_price, net_qty, 53.333333333333333333, 50);
    EXPECT_EQ(net_qty[1], 100);  // Количество в long должно быть 100
    EXPECT_DOUBLE_EQ(avg_price[1],
                     53.333333333333333333);  // Средняя цена не изменится, т.к.
                                              // не изменяется количество

    // Удаляем 50 единиц по цене 55 (для short)
    strategy_->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                      avg_price, net_qty, 55.0, -50);
    EXPECT_EQ(net_qty[0], 0);           // Количество в short должно стать 0
    EXPECT_DOUBLE_EQ(avg_price[0], 0);  // Средняя цена в short должна быть 0

    // Удаляем все из long
    strategy_->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                      avg_price, net_qty, 50.0, 100);
    EXPECT_EQ(net_qty[1], 0);           // Количество в long должно быть 0
    EXPECT_DOUBLE_EQ(avg_price[1], 0);  // Средняя цена в long должна быть 0
}

TEST_F(HedgedPositionStrategyTest, AddAndRemoveBasic) {
    Price avg_price[2] = {0.0, 0.0};
    Qty net_qty[2]     = {0.0, 0.0};

    // Покупаем 10 контрактов по цене 100 (LONG)
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 100.0, 10.0);
    EXPECT_DOUBLE_EQ(avg_price[1], 100.0);
    EXPECT_DOUBLE_EQ(net_qty[1], 10.0);

    // Продаем 5 контрактов по цене 110 (фиксируем прибыль)
    strategy_->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                      avg_price, net_qty, 110.0, 5.0);
    auto [status1, pnl1] = ptr_realized_storage_->GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status1, true);
    EXPECT_DOUBLE_EQ(pnl1, 5.0 * (110.0 - 100.0));  // 5 * 10 = 50
    EXPECT_DOUBLE_EQ(avg_price[1], 90.0);           //(100 * 10 - 110*5)/5
    EXPECT_DOUBLE_EQ(net_qty[1], 5.0);

    // Продаем оставшиеся 5 контрактов по 90 (убыток)
    strategy_->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                      avg_price, net_qty, 90.0, 5.0);
    auto [status2, pnl2] = ptr_realized_storage_->GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_DOUBLE_EQ(pnl2, 50);  // (90-90)*5 + previous 50
    EXPECT_DOUBLE_EQ(avg_price[1], 0);
    EXPECT_DOUBLE_EQ(net_qty[1], 0.0);

    // Шортим 10 контрактов по цене 95 (SHORT)
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 95.0, -10.0);
    EXPECT_DOUBLE_EQ(avg_price[0], 95.0);
    EXPECT_DOUBLE_EQ(net_qty[0], -10.0);

    // Покупаем 10 контрактов по 90 (фиксируем прибыль на шорте)
    strategy_->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                      avg_price, net_qty, 90.0, -10.0);
    auto [status3, pnl3] = ptr_realized_storage_->GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_DOUBLE_EQ(pnl3, 100);  // 10 * 5 = 50 + previous 50
    EXPECT_DOUBLE_EQ(net_qty[0], 0.0);
}

TEST_F(HedgedPositionStrategyTest, FlipPosition) {
    Price avg_price[2] = {0.0, 0.0};
    Qty net_qty[2]     = {0.0, 0.0};

    // Лонг 10 по 100
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 100.0, 10.0);

    // Продажа 15 по 110 (переворот в шорт)
    strategy_->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                      avg_price, net_qty, 110.0, 15.0);
    auto [status, pnl] = ptr_realized_storage_->GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_DOUBLE_EQ(pnl, 10.0 * (110.0 - 100.0));  // 10 * 10 = 100
    EXPECT_DOUBLE_EQ(net_qty[1], 0.0);
    EXPECT_DOUBLE_EQ(net_qty[0], 5.0);
    EXPECT_DOUBLE_EQ(avg_price[0], 110.0);
}

TEST_F(HedgedPositionStrategyTest, UnrealizedPnlLongTest) {
    Price avg_price[2] = {0.0, 0.0};
    Qty net_qty[2]     = {0.0, 0.0};

    // LONG: покупаем 10 контрактов по цене 100
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 100.0, 10.0);

    // Обновляем рыночную цену до 110
    ptr_unrealized_storage_->UpdateBBO(common::ExchangeId::kBinance,
                                       aos::TradingPair::kBTCUSDT, 90, 110.0);

    // Должна быть нереализованная прибыль: (110 - 100) * 10 = 100
    auto [status, unrealized_pnl] = ptr_unrealized_storage_->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(unrealized_pnl, -100.0);
}

TEST_F(HedgedPositionStrategyTest, UnrealizedPnlShortTest) {
    Price avg_price[2] = {0.0, 0.0};
    Qty net_qty[2]     = {0.0, 0.0};

    // SHORT: шортим 10 контрактов по 120
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 120.0, -10.0);

    // Обновляем рыночную цену до 110
    ptr_unrealized_storage_->UpdateBBO(common::ExchangeId::kBinance,
                                       aos::TradingPair::kBTCUSDT, 90, 110.0);

    // Должна быть нереализованная прибыль: (120 - 110) * 10 = 100
    auto [status, unrealized_pnl] = ptr_unrealized_storage_->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(unrealized_pnl, 100.0);  //(120-110)*10 = 100
}

TEST_F(HedgedPositionStrategyTest, UnrealizedPnlZeroIfNoPrice) {
    // Ничего не делаем, но пробуем получить нереализованную прибыль
    auto [status, unrealized_pnl] = ptr_unrealized_storage_->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_FALSE(status);  // Не должно быть данных
}

TEST_F(HedgedPositionStrategyTest, UnrealizedPnlNotExistIfNoPrice) {
    Price avg_price[2] = {0.0, 0.0};
    Qty net_qty[2]     = {0.0, 0.0};

    // Обновляем рыночную цену до 110
    ptr_unrealized_storage_->UpdateBBO(common::ExchangeId::kBinance,
                                       aos::TradingPair::kBTCUSDT, 90, 110.0);

    // Должна быть нереализованная прибыль: (110 - 100) * 10 = 100
    auto [status, unrealized_pnl] = ptr_unrealized_storage_->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_FALSE(status);
}

TEST_F(HedgedPositionStrategyTest, UnrealizedPnlNotExistIfNoPosition) {
    Price avg_price[2] = {0.0, 0.0};
    Qty net_qty[2]     = {0.0, 0.0};

    // Обновляем рыночную цену до 110
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 120.0, -10.0);

    // Должна быть нереализованная прибыль: (110 - 100) * 10 = 100
    auto [status, unrealized_pnl] = ptr_unrealized_storage_->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_FALSE(status);
}

TEST_F(HedgedPositionStrategyTest, UnrealizedPnlAddLongAddShort) {
    Price avg_price[2] = {0.0, 0.0};
    Qty net_qty[2]     = {0.0, 0.0};

    // LONG: покупаем 10 контрактов по цене 100
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 100.0, 10.0);

    // Обновляем рыночную цену до 110
    ptr_unrealized_storage_->UpdateBBO(common::ExchangeId::kBinance,
                                       aos::TradingPair::kBTCUSDT, 90, 110.0);

    // Должна быть нереализованная прибыль: (110 - 100) * 10 = 100
    auto [status1, unrealized_pnl1] = ptr_unrealized_storage_->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status1);
    EXPECT_DOUBLE_EQ(unrealized_pnl1, -100.0);

    // SHORT: шортим 10 контрактов по 120
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 120.0, -10.0);

    // Должна быть нереализованная прибыль: (120 - 110) * 10 = 100
    auto [status2, unrealized_pnl2] = ptr_unrealized_storage_->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status2);
    EXPECT_DOUBLE_EQ(unrealized_pnl2, 0);  //(-100 + 100) = 0
}

TEST_F(HedgedPositionStrategyTest, UnrealizedPnlAddLongAddShortRemoveLong) {
    Price avg_price[2] = {0.0, 0.0};
    Qty net_qty[2]     = {0.0, 0.0};

    // LONG: покупаем 10 контрактов по цене 100
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 100.0, 10.0);

    // Обновляем рыночную цену до 110
    ptr_unrealized_storage_->UpdateBBO(common::ExchangeId::kBinance,
                                       aos::TradingPair::kBTCUSDT, 90, 110.0);

    // Должна быть нереализованная прибыль: (110 - 100) * 10 = 100
    auto [status1, unrealized_pnl1] = ptr_unrealized_storage_->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status1);
    EXPECT_DOUBLE_EQ(unrealized_pnl1, -100.0);

    // SHORT: шортим 10 контрактов по 120
    strategy_->Add(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                   avg_price, net_qty, 120.0, -10.0);

    // Должна быть нереализованная прибыль: (120 - 110) * 10 = 100
    auto [status2, unrealized_pnl2] = ptr_unrealized_storage_->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status2);
    EXPECT_DOUBLE_EQ(unrealized_pnl2, 0);  //(-100 + 100) = 0

    strategy_->Remove(common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT,
                      avg_price, net_qty, 100.0, 5.0);
    auto [status3, unrealized_pnl3] = ptr_unrealized_storage_->GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status3);
    EXPECT_DOUBLE_EQ(unrealized_pnl3, 50);  //(-50 + 100) = 0
}

int main(int argc, char** argv) {
    fmtlog::setLogLevel(
        fmtlog::LogLevel::OFF);  // полностью отключит логгирование
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}