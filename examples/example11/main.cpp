#include <iostream>

#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "aos/position/position.h"
#include "aos/position_storage/position_storage_by_pair/position_storage_by_pair.h"
#include "aos/position_strategy/position_strategy.h"
#include "aot/common/mem_pool.h"
#include "boost/intrusive_ptr.hpp"

int main() {
    {
        using Price = double;
        using Qty   = double;
        using Uid   = size_t;
        using RealizedPnlStorageT =
            aos::impl::RealizedPnlStorage<Price, Qty,
                                          common::MemoryPoolNotThreadSafety>;
        using RealizedPnlStorageContainerT =
            aos::impl::RealizedPnlStorageContainer<
                Price, Qty, common::MemoryPoolNotThreadSafety,
                RealizedPnlStorageT>;

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

        aos::impl::PositionStrategyContainer<
            aos::impl::NetPositionStrategy<Price, Qty,
                                           common::MemoryPoolNotThreadSafety>,
            aos::impl::PositionStrategyBuilder<
                aos::impl::NetPositionStrategy<
                    Price, Qty, common::MemoryPoolNotThreadSafety>,
                Price, Qty, common::MemoryPoolNotThreadSafety>,
            Price, Qty, common::MemoryPoolNotThreadSafety,
            RealizedPnlStorageContainerT, UnrealizedPnlStorageContainerT>
            position_strategy_containter(1);

        auto position_strategy = position_strategy_containter.Build();
        aos::impl::NetPosition<Price, Qty, common::MemoryPoolNotThreadSafety>
            long_position(position_strategy);  // Длинная позиция
        // Добавить 100 единиц по цене 50
        long_position.AddPosition(common::ExchangeId::kBinance, {2, 1}, 50.0,
                                  100);
        std::cout << "Длинная позиция: Чистая позиция: "
                  << long_position.GetPosition()
                  << "\nСредняя цена: " << long_position.GetAveragePrice()
                  << "\n";

        // Добавить 50 единиц по цене 60
        long_position.AddPosition(common::ExchangeId::kBinance, {2, 1}, 60.0,
                                  50);
        std::cout << "Длинная позиция: Чистая позиция: "
                  << long_position.GetPosition()
                  << "\nСредняя цена: " << long_position.GetAveragePrice()
                  << "\n";

        // Удалить 230 единиц (позиция станет отрицательной)
        long_position.RemovePosition(common::ExchangeId::kBinance, {2, 1}, 55.0,
                                     230);
        std::cout << "Длинная позиция после Remove: Чистая позиция: "
                  << long_position.GetPosition()
                  << "\nСредняя цена: " << long_position.GetAveragePrice()
                  << "\n";
    }
    {
        using Price = double;
        using Qty   = double;
        using Uid   = size_t;
        using RealizedPnlStorageT =
            aos::impl::RealizedPnlStorage<Price, Qty,
                                          common::MemoryPoolNotThreadSafety>;
        using RealizedPnlStorageContainerT =
            aos::impl::RealizedPnlStorageContainer<
                Price, Qty, common::MemoryPoolNotThreadSafety,
                RealizedPnlStorageT>;
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

        aos::impl::PositionStrategyContainer<
            aos::impl::NetPositionStrategy<Price, Qty,
                                           common::MemoryPoolNotThreadSafety>,
            aos::impl::PositionStrategyBuilder<
                aos::impl::NetPositionStrategy<
                    Price, Qty, common::MemoryPoolNotThreadSafety>,
                Price, Qty, common::MemoryPoolNotThreadSafety>,
            Price, Qty, common::MemoryPoolNotThreadSafety,
            RealizedPnlStorageContainerT, UnrealizedPnlStorageContainerT>
            position_strategy_containter(1);

        auto position_strategy = position_strategy_containter.Build();
        aos::impl::NetPosition<Price, Qty, common::MemoryPoolNotThreadSafety>
            short_position(position_strategy);  // Короткая позиция (шорт)

        // Открыть шорт на 100 единиц по цене 50
        short_position.AddPosition(common::ExchangeId::kBinance, {2, 1}, 50.0,
                                   -100);
        std::cout << "Шорт-позиция: Чистая позиция: "
                  << short_position.GetPosition()
                  << "\nСредняя цена: " << short_position.GetAveragePrice()
                  << "\n";

        // Добавить ещё 50 единиц в шорт по цене 60
        short_position.AddPosition(common::ExchangeId::kBinance, {2, 1}, 60.0,
                                   -50);
        std::cout << "Шорт-позиция: Чистая позиция: "
                  << short_position.GetPosition()
                  << "\nСредняя цена: " << short_position.GetAveragePrice()
                  << "\n";

        // Покрыть часть шорта на 80 единиц по цене 55
        short_position.RemovePosition(common::ExchangeId::kBinance, {2, 1},
                                      55.0, -80);
        std::cout << "Шорт-позиция после покрытия: Чистая позиция: "
                  << short_position.GetPosition()
                  << "\nСредняя цена: " << short_position.GetAveragePrice()
                  << "\n";

        // Полностью закрыть шорт (закрываем оставшиеся 70 единиц)
        short_position.RemovePosition(common::ExchangeId::kBinance, {2, 1},
                                      58.0, -70);
        std::cout << "Шорт-позиция после полного закрытия: Чистая позиция: "
                  << short_position.GetPosition()
                  << "\nСредняя цена: " << short_position.GetAveragePrice()
                  << "\n";
    }
    {
        using Price = double;
        using Qty   = double;
        using RealizedPnlStorageT =
            aos::impl::RealizedPnlStorage<Price, Qty,
                                          common::MemoryPoolNotThreadSafety>;
        using RealizedPnlStorageContainerT =
            aos::impl::RealizedPnlStorageContainer<
                Price, Qty, common::MemoryPoolNotThreadSafety,
                RealizedPnlStorageT>;
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
        aos::impl::PositionStrategyContainer<
            aos::impl::HedgedPositionStrategy<
                Price, Qty, common::MemoryPoolNotThreadSafety>,
            aos::impl::PositionStrategyBuilder<
                aos::impl::HedgedPositionStrategy<
                    Price, Qty, common::MemoryPoolNotThreadSafety>,
                Price, Qty, common::MemoryPoolNotThreadSafety>,
            Price, Qty, common::MemoryPoolNotThreadSafety,
            RealizedPnlStorageContainerT, UnrealizedPnlStorageContainerT>
            position_strategy_containter(1);

        auto position_strategy = position_strategy_containter.Build();

        aos::impl::HedgePosition<Price, Qty, common::MemoryPoolNotThreadSafety>
            position(position_strategy);  // Позиция с хеджированием (длинная и
                                          // короткая)

        // Логгирование добавления позиции (long)
        logi("Adding 100 units at price 50.0 (long position)");
        position.AddPosition(common::ExchangeId::kBinance, {2, 1}, 50.0, 100);
        logi("Position after Add: Net position: {}, Average price: {}",
             position.GetPosition(), position.GetAveragePrice());

        // Логгирование добавления позиции (long)
        logi("Adding 50 units at price 60.0 (long position)");
        position.AddPosition(common::ExchangeId::kBinance, {2, 1}, 60.0, 50);
        logi("Position after Add: Net position: {}, Average price: {}",
             position.GetPosition(), position.GetAveragePrice());

        // Логгирование добавления позиции (short)
        logi("Adding 50 units at price -60.0 (short position)");
        position.AddPosition(common::ExchangeId::kBinance, {2, 1}, 60.0, -50);
        logi("Position after Add: Net position: {}, Average price: {}",
             position.GetPosition(), position.GetAveragePrice());

        // Логгирование удаления позиции
        logi("Removing 230 units at price 55.0");
        position.RemovePosition(common::ExchangeId::kBinance, {2, 1}, 55.0,
                                155);
        logi("Position after Remove: Net position: {}, Average price: {}",
             position.GetPosition(), position.GetAveragePrice());
    }
    fmtlog::poll();
    return 0;
}