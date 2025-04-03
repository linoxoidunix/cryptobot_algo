#include <gtest/gtest.h>

#include <memory>

#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/position_strategy/position_strategy.h"
#include "aot/common/mem_pool.h"
// Helper function to initialize the parser manager

using Price = double;
using Qty   = double;

// class HedgedPositionStrategyTest : public ::testing::Test {
//   protected:
//     aos::impl::RealizedPnlStorageContainer<Price, Qty,
//     common::MemoryPoolNotThreadSafety>
//         realized_pnl{1};
//     auto ptr = realized_pnl.Build();
//     aos::impl::HedgedPositionStrategy<Price, Qty,
//     common::MemoryPoolNotThreadSafety> strategy(ptr);
// };

class HedgedPositionStrategyTest : public ::testing::Test {
  protected:
    aos::impl::RealizedPnlStorageContainer<Price, Qty,
                                           common::MemoryPoolNotThreadSafety>
        realized_pnl{1};
    boost::intrusive_ptr<aos::impl::RealizedPnlStorage<
        Price, Qty, common::MemoryPoolNotThreadSafety>>
        ptr;
    std::unique_ptr<aos::impl::HedgedPositionStrategy<
        Price, Qty, common::MemoryPoolNotThreadSafety>>
        strategy;

    void SetUp() override {
        ptr      = realized_pnl.Build();
        strategy = std::make_unique<aos::impl::HedgedPositionStrategy<
            Price, Qty, common::MemoryPoolNotThreadSafety>>(ptr);
    }
};

TEST_F(HedgedPositionStrategyTest, AddAndRemoveTest) {
    using namespace aos::impl;
    using Price        = double;
    using Qty          = double;

    // Инициализация переменных
    Price avg_price[2] = {0, 0};  // Средняя цена для long (1) и short (0)
    Qty net_qty[2]     = {0, 0};  // Количество для long и short

    // Добавляем позицию с положительной ценой (long)
    strategy->Add(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                  50.0, 100);
    EXPECT_EQ(net_qty[1], 100);  // Количество в long должно быть 100
    EXPECT_DOUBLE_EQ(avg_price[1],
                     50.0);  // Средняя цена в long должна быть 50.0

    // Добавляем ещё 50 единиц с более высокой ценой (long)
    strategy->Add(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                  60.0, 50);
    EXPECT_EQ(net_qty[1], 150);  // Количество в long должно быть 150
    EXPECT_DOUBLE_EQ(avg_price[1],
                     53.333333333333333333);  // Средняя цена должна быть
                                              // (50*100 + 60*50) / 150
                                              // = 53.33333333333333333

    // Добавляем позицию с отрицательной ценой (short)
    strategy->Add(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                  60.0, -50);
    EXPECT_EQ(net_qty[0], -50);  // Количество в short должно быть 50
    EXPECT_DOUBLE_EQ(avg_price[0],
                     60.0);  // Средняя цена в short должна быть 60.0

    // Удаляем 50 единиц по цене 55 (для long)
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     53.333333333333333333, 50);
    EXPECT_EQ(net_qty[1], 100);  // Количество в long должно быть 100
    EXPECT_DOUBLE_EQ(avg_price[1],
                     53.333333333333333333);  // Средняя цена не изменится, т.к.
                                              // не изменяется количество

    // Удаляем 50 единиц по цене 55 (для short)
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     55.0, -50);
    EXPECT_EQ(net_qty[0], 0);           // Количество в short должно стать 0
    EXPECT_DOUBLE_EQ(avg_price[0], 0);  // Средняя цена в short должна быть 0

    // Удаляем все из long
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     50.0, 100);
    EXPECT_EQ(net_qty[1], 0);           // Количество в long должно быть 0
    EXPECT_DOUBLE_EQ(avg_price[1], 0);  // Средняя цена в long должна быть 0
}

TEST_F(HedgedPositionStrategyTest, AddAndRemoveBasic) {
    Price avg_price[2] = {0.0, 0.0};
    Qty net_qty[2]     = {0.0, 0.0};

    // Покупаем 10 контрактов по цене 100 (LONG)
    strategy->Add(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                  100.0, 10.0);
    EXPECT_DOUBLE_EQ(avg_price[1], 100.0);
    EXPECT_DOUBLE_EQ(net_qty[1], 10.0);

    // Продаем 5 контрактов по цене 110 (фиксируем прибыль)
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     110.0, 5.0);
    auto [status1, pnl1] =
        ptr->GetRealizedPnl(common::ExchangeId::kBinance, {2, 1});
    EXPECT_EQ(status1, true);
    EXPECT_DOUBLE_EQ(pnl1, 5.0 * (110.0 - 100.0));  // 5 * 10 = 50
    EXPECT_DOUBLE_EQ(avg_price[1], 90.0);           //(100 * 10 - 110*5)/5
    EXPECT_DOUBLE_EQ(net_qty[1], 5.0);

    // Продаем оставшиеся 5 контрактов по 90 (убыток)
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     90.0, 5.0);
    auto [status2, pnl2] =
        ptr->GetRealizedPnl(common::ExchangeId::kBinance, {2, 1});
    EXPECT_DOUBLE_EQ(pnl2, 50);  // (90-90)*5 + previous 50
    EXPECT_DOUBLE_EQ(avg_price[1], 0);
    EXPECT_DOUBLE_EQ(net_qty[1], 0.0);

    // Шортим 10 контрактов по цене 95 (SHORT)
    strategy->Add(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                  95.0, -10.0);
    EXPECT_DOUBLE_EQ(avg_price[0], 95.0);
    EXPECT_DOUBLE_EQ(net_qty[0], -10.0);

    // Покупаем 10 контрактов по 90 (фиксируем прибыль на шорте)
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     90.0, -10.0);
    auto [status3, pnl3] =
        ptr->GetRealizedPnl(common::ExchangeId::kBinance, {2, 1});
    EXPECT_DOUBLE_EQ(pnl3, 100);  // 10 * 5 = 50 + previous 50
    EXPECT_DOUBLE_EQ(net_qty[0], 0.0);
}

TEST_F(HedgedPositionStrategyTest, FlipPosition) {
    Price avg_price[2] = {0.0, 0.0};
    Qty net_qty[2]     = {0.0, 0.0};

    // Лонг 10 по 100
    strategy->Add(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                  100.0, 10.0);

    // Продажа 15 по 110 (переворот в шорт)
    strategy->Remove(common::ExchangeId::kBinance, {2, 1}, avg_price, net_qty,
                     110.0, 15.0);
    auto [status, pnl] =
        ptr->GetRealizedPnl(common::ExchangeId::kBinance, {2, 1});
    EXPECT_DOUBLE_EQ(pnl, 10.0 * (110.0 - 100.0));  // 10 * 10 = 100
    EXPECT_DOUBLE_EQ(net_qty[1], 0.0);
    EXPECT_DOUBLE_EQ(net_qty[0], 5.0);
    EXPECT_DOUBLE_EQ(avg_price[0], 110.0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}