#include <iostream>

#include "aos/position/position.h"
#include "aos/position_strategy/position_strategy.h"
#include "aot/common/mem_pool.h"
#include "boost/intrusive_ptr.hpp"

int main() {
    {
        using Price = double;
        using Qty   = double;
        using Uid   = size_t;
        aos::impl::PositionStrategyContainer<
            aos::impl::NetPositionStrategy<Price, Qty,
                                           common::MemoryPoolNotThreadSafety>,
            aos::impl::PositionStrategyBuilder<
                aos::impl::NetPositionStrategy<
                    Price, Qty, common::MemoryPoolNotThreadSafety>,
                Price, Qty, common::MemoryPoolNotThreadSafety>,
            Price, Qty, common::MemoryPoolNotThreadSafety>
            position_strategy_containter(1);

        auto position_strategy = position_strategy_containter.Build();
        aos::impl::Position<Price, Qty, common::MemoryPoolNotThreadSafety>
            long_position(position_strategy);  // Длинная позиция
        // Добавить 100 единиц по цене 50
        long_position.AddPosition(50.0, 100);
        std::cout << "Длинная позиция: Чистая позиция: "
                  << long_position.GetPosition()
                  << "\nСредняя цена: " << long_position.GetAveragePrice()
                  << "\n";

        // Добавить 50 единиц по цене 60
        long_position.AddPosition(60.0, 50);
        std::cout << "Длинная позиция: Чистая позиция: "
                  << long_position.GetPosition()
                  << "\nСредняя цена: " << long_position.GetAveragePrice()
                  << "\n";

        // Удалить 230 единиц (позиция станет отрицательной)
        long_position.RemovePosition(55.0, 230);
        std::cout << "Длинная позиция после Remove: Чистая позиция: "
                  << long_position.GetPosition()
                  << "\nСредняя цена: " << long_position.GetAveragePrice()
                  << "\n";
    }
    fmtlog::poll();
    return 0;
}