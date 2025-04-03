#include <iostream>

#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/position/position.h"
#include "aos/position_storage/position_storage_by_pair/position_storage_by_pair.h"
#include "aos/position_strategy/position_strategy.h"
#include "aos/position_tracker/position_tracker.h"
#include "aot/common/mem_pool.h"
#include "boost/intrusive_ptr.hpp"

int main() {
    {
        using Price = double;
        using Qty   = double;
        using Uid   = size_t;
        aos::impl::RealizedPnlStorageContainer<
            Price, Qty, common::MemoryPoolNotThreadSafety>
            realized_pnl_container{1};
        auto realized_pnl = realized_pnl_container.Build();

        aos::impl::PositionStrategyContainer<
            aos::impl::NetPositionStrategy<Price, Qty,
                                           common::MemoryPoolNotThreadSafety>,
            aos::impl::PositionStrategyBuilder<
                aos::impl::NetPositionStrategy<
                    Price, Qty, common::MemoryPoolNotThreadSafety>,
                Price, Qty, common::MemoryPoolNotThreadSafety>,
            Price, Qty, common::MemoryPoolNotThreadSafety>
            position_strategy_containter(1, realized_pnl);
        using Position =
            aos::impl::NetPosition<Price, Qty,
                                   common::MemoryPoolNotThreadSafety>;
        using PositionStrategy =
            aos::impl::NetPositionStrategy<Price, Qty,
                                           common::MemoryPoolNotThreadSafety>;
        auto position_strategy = position_strategy_containter.Build();

        aos::impl::PositionStorageContainer<
            Price, Qty, common::MemoryPoolNotThreadSafety,
            aos::impl::PositionStorageByPair<Price, Qty,
                                             common::MemoryPoolNotThreadSafety,
                                             Position, PositionStrategy>,
            PositionStrategy>
            position_storage_container(1, position_strategy);

        auto position_storage = position_storage_container.Build();

        aos::impl::PositionTracker<Price, Qty,
                                   common::MemoryPoolNotThreadSafety>
            position_tracker;
        position_tracker.AddObserver(position_storage);
        // Добавить 100 единиц по цене 50
        position_tracker.AddPosition(common::ExchangeId::kBinance, {2, 1}, 50.0,
                                     100);
        // std::cout << "Длинная позиция: Чистая позиция: "
        //           << long_position.GetPosition()
        //           << "\nСредняя цена: " << long_position.GetAveragePrice()
        //           << "\n";
        position_tracker.AddPosition(common::ExchangeId::kBinance, {2, 1}, 60.0,
                                     50);
        position_tracker.RemovePosition(common::ExchangeId::kBinance, {2, 1},
                                        70.0, 150);
        // // Добавить 50 единиц по цене 60
        // long_position.AddPosition(60.0, 50);
        // std::cout << "Длинная позиция: Чистая позиция: "
        //           << long_position.GetPosition()
        //           << "\nСредняя цена: " << long_position.GetAveragePrice()
        //           << "\n";

        // // Удалить 230 единиц (позиция станет отрицательной)
        // long_position.RemovePosition(55.0, 230);
        // std::cout << "Длинная позиция после Remove: Чистая позиция: "
        //           << long_position.GetPosition()
        //           << "\nСредняя цена: " << long_position.GetAveragePrice()
        //           << "\n";
    }

    fmtlog::poll();
    return 0;
}