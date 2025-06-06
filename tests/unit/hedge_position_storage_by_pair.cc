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
#include "aos/common/mem_pool.h"
// Helper function to initialize the parser manager

using Price = double;
using Qty   = double;

class HedgedPositionSorageByPairDefaultTest : public ::testing::Test {
  protected:
    aos::impl::RealizedPnlCalculatorDefault<Price, Qty> realized_pnl_calculator;
    aos::impl::RealizedPnlStorageDefault<Price, Qty> realized_pnl_storage;
    aos::impl::UnRealizedPnlCalculatorDefault<Price, Qty>
        unrealized_pnl_calculator;
    aos::impl::HedgedUnRealizedPnlStorageDefault<Price, Qty>
        un_realized_pnl_storage;

    aos::impl::HedgedPositionStrategyDefault<Price, Qty> strategy;
    aos::impl::PositionStorageByPair<
        Price, Qty, aos::impl::HedgedPositionDefault<Price, Qty>>
        position_storage_by_pair;
    void SetUp() override {}

  public:
    HedgedPositionSorageByPairDefaultTest()
        : realized_pnl_storage(realized_pnl_calculator),
          un_realized_pnl_storage(unrealized_pnl_calculator),
          strategy(realized_pnl_storage, un_realized_pnl_storage),
          position_storage_by_pair([this]() {
              return aos::impl::HedgedPositionDefault<Price, Qty>(strategy);
          }) {
        // безопасно инициализировать поля
    }
};

TEST_F(HedgedPositionSorageByPairDefaultTest, AddAndRemoveTest) {
    // Добавляем позицию с положительной ценой (long)
    position_storage_by_pair.AddPosition(common::ExchangeId::kBinance,
                                         aos::TradingPair::kBTCUSDT, 50.0, 100);
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());

    EXPECT_EQ(position.value().get().GetLongPosition(),
              100);  // Количество в long должно быть 100
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageLongPrice(),
                     50.0);  // Средняя цена в long должна быть 50.0

    // Добавляем ещё 50 единиц с более высокой ценой (long)
    position_storage_by_pair.AddPosition(common::ExchangeId::kBinance,
                                         aos::TradingPair::kBTCUSDT, 60.0, 50);
    EXPECT_EQ(position.value().get().GetLongPosition(),
              150);  // Количество в long должно быть 150
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageLongPrice(),
                     53.333333333333333333);  // Средняя цена должна быть
                                              // (50*100 + 60*50) / 150
                                              // = 53.33333333333333333

    // Добавляем позицию с отрицательной ценой (short)
    position_storage_by_pair.AddPosition(common::ExchangeId::kBinance,
                                         aos::TradingPair::kBTCUSDT, 60.0, -50);
    EXPECT_EQ(position.value().get().GetShortPosition(),
              -50);  // Количество в short должно быть 50
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageShortPrice(),
                     60.0);  // Средняя цена в short должна быть 60.0

    // Удаляем 50 единиц по цене 55 (для long)
    position_storage_by_pair.RemovePosition(common::ExchangeId::kBinance,
                                            aos::TradingPair::kBTCUSDT,
                                            53.333333333333333333, 50);
    EXPECT_EQ(position.value().get().GetLongPosition(),
              100);  // Количество в long должно быть 100
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageLongPrice(),
                     53.333333333333333333);  // Средняя цена не изменится, т.к.
                                              // не изменяется количество

    // Удаляем 50 единиц по цене 55 (для short)
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 55.0, -50);
    EXPECT_EQ(position.value().get().GetShortPosition(),
              0);  // Количество в short должно стать 0
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageShortPrice(),
                     0);  // Средняя цена в short должна быть 0

    // Удаляем все из long
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 50.0, 100);
    EXPECT_EQ(position.value().get().GetLongPosition(),
              0);  // Количество в long должно быть 0
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageLongPrice(),
                     0);  // Средняя цена в long должна быть 0
}

TEST_F(HedgedPositionSorageByPairDefaultTest, AddAndRemoveBasic) {
    // Покупаем 10 контрактов по цене 100 (LONG)
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 10.0);
    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());

    EXPECT_DOUBLE_EQ(position.value().get().GetAverageLongPrice(), 100.0);
    EXPECT_DOUBLE_EQ(position.value().get().GetLongPosition(), 10.0);

    // Продаем 5 контрактов по цене 110 (фиксируем прибыль)
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 110.0, 5.0);
    auto [status1, pnl1] = realized_pnl_storage.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_EQ(status1, true);
    EXPECT_DOUBLE_EQ(pnl1, 5.0 * (110.0 - 100.0));  // 5 * 10 = 50
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageLongPrice(),
                     90.0);  //(100 * 10 - 110*5)/5
    EXPECT_DOUBLE_EQ(position.value().get().GetLongPosition(), 5.0);

    // Продаем оставшиеся 5 контрактов по 90 (убыток)
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 90.0, 5.0);
    auto [status2, pnl2] = realized_pnl_storage.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_DOUBLE_EQ(pnl2, 50);  // (90-90)*5 + previous 50
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageLongPrice(), 0);
    EXPECT_DOUBLE_EQ(position.value().get().GetLongPosition(), 0.0);

    // Шортим 10 контрактов по цене 95 (SHORT)
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 95.0, -10.0);
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageShortPrice(), 95.0);
    EXPECT_DOUBLE_EQ(position.value().get().GetShortPosition(), -10.0);

    // Покупаем 10 контрактов по 90 (фиксируем прибыль на шорте)
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 90.0, -10.0);
    auto [status3, pnl3] = realized_pnl_storage.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_DOUBLE_EQ(pnl3, 100);  // 10 * 5 = 50 + previous 50
    EXPECT_DOUBLE_EQ(position.value().get().GetShortPosition(), 0.0);
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageShortPrice(), 0);
}

TEST_F(HedgedPositionSorageByPairDefaultTest, FlipPosition) {
    // Лонг 10 по 100
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 10.0);

    // Продажа 15 по 110 (переворот в шорт)
    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 110.0, 15.0);
    auto [status, pnl] = realized_pnl_storage.GetRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);

    auto position = position_storage_by_pair.GetPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(position.has_value());

    EXPECT_DOUBLE_EQ(pnl, 10.0 * (110.0 - 100.0));  // 10 * 10 = 100
    EXPECT_DOUBLE_EQ(position.value().get().GetLongPosition(), 0.0);
    EXPECT_DOUBLE_EQ(position.value().get().GetShortPosition(), 5.0);
    EXPECT_DOUBLE_EQ(position.value().get().GetAverageShortPrice(), 110.0);
}

TEST_F(HedgedPositionSorageByPairDefaultTest, UnrealizedPnlLongTest) {
    // LONG: покупаем 10 контрактов по цене 100
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 10.0);

    // Обновляем рыночную цену до 110
    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 90, 110.0);

    // Должна быть нереализованная прибыль: (110 - 100) * 10 = 100
    auto [status, unrealized_pnl] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(unrealized_pnl, -100.0);
}

TEST_F(HedgedPositionSorageByPairDefaultTest, UnrealizedPnlShortTest) {
    // SHORT: шортим 10 контрактов по 120
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, -10.0);

    // Обновляем рыночную цену до 110
    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 90, 110.0);

    // Должна быть нереализованная прибыль: (120 - 110) * 10 = 100
    auto [status, unrealized_pnl] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status);
    EXPECT_DOUBLE_EQ(unrealized_pnl, 100.0);  //(120-110)*10 = 100
}

TEST_F(HedgedPositionSorageByPairDefaultTest, UnrealizedPnlZeroIfNoPrice) {
    // Ничего не делаем, но пробуем получить нереализованную прибыль
    auto [status, unrealized_pnl] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_FALSE(status);  // Не должно быть данных
}

TEST_F(HedgedPositionSorageByPairDefaultTest,
       UnrealizedPnlNotExistIfNoPosition) {
    // Обновляем рыночную цену до 110
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, -10.0);

    // Должна быть нереализованная прибыль: (110 - 100) * 10 = 100
    auto [status, unrealized_pnl] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_FALSE(status);
}

TEST_F(HedgedPositionSorageByPairDefaultTest, UnrealizedPnlAddLongAddShort) {
    // LONG: покупаем 10 контрактов по цене 100
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 10.0);

    // Обновляем рыночную цену до 110
    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 90, 110.0);

    // Должна быть нереализованная прибыль: (110 - 100) * 10 = 100
    auto [status1, unrealized_pnl1] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status1);
    EXPECT_DOUBLE_EQ(unrealized_pnl1, -100.0);

    // SHORT: шортим 10 контрактов по 120
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, -10.0);

    // Должна быть нереализованная прибыль: (120 - 110) * 10 = 100
    auto [status2, unrealized_pnl2] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status2);
    EXPECT_DOUBLE_EQ(unrealized_pnl2, 0);  //(-100 + 100) = 0
}

TEST_F(HedgedPositionSorageByPairDefaultTest,
       UnrealizedPnlAddLongAddShortRemoveLong) {
    // LONG: покупаем 10 контрактов по цене 100
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 10.0);

    // Обновляем рыночную цену до 110
    un_realized_pnl_storage.UpdateBBO(common::ExchangeId::kBinance,
                                      aos::TradingPair::kBTCUSDT, 90, 110.0);

    // Должна быть нереализованная прибыль: (110 - 100) * 10 = 100
    auto [status1, unrealized_pnl1] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status1);
    EXPECT_DOUBLE_EQ(unrealized_pnl1, -100.0);

    // SHORT: шортим 10 контрактов по 120
    position_storage_by_pair.AddPosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 120.0, -10.0);

    // Должна быть нереализованная прибыль: (120 - 110) * 10 = 100
    auto [status2, unrealized_pnl2] = un_realized_pnl_storage.GetUnRealizedPnl(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT);
    EXPECT_TRUE(status2);
    EXPECT_DOUBLE_EQ(unrealized_pnl2, 0);  //(-100 + 100) = 0

    position_storage_by_pair.RemovePosition(
        common::ExchangeId::kBinance, aos::TradingPair::kBTCUSDT, 100.0, 5.0);
    auto [status3, unrealized_pnl3] = un_realized_pnl_storage.GetUnRealizedPnl(
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