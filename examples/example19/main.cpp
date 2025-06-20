#include "aoe/bybit/execution_event/types.h"
#include "aoe/bybit/execution_watcher/execution_watcher.h"
#include "aos/common/mem_pool.h"
#include "aos/pnl/realized_calculator/pnl_realized_calculator.h"
#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "aos/position_storage/position_storage_by_pair/position_storage_by_pair.h"
#include "aos/position_strategy/position_strategy.h"

int main() {
    {
        using HashT     = std::size_t;
        using Price     = double;
        using Qty       = double;
        // using namespace aos::impl;
        using PositionT = aos::impl::NetPositionDefault<Price, Qty>;
        aos::impl::RealizedPnlCalculatorDefault<Price, Qty>
            realized_pnl_calculator;
        aos::impl::RealizedPnlStorageDefault<Price, Qty> realized_pnl_storage(
            realized_pnl_calculator);
        aos::impl::UnRealizedPnlCalculatorDefault<Price, Qty>
            unrealized_pnl_calculator;
        aos::impl::NetUnRealizedPnlStorageDefault<Price, Qty>
            un_realized_pnl_storage(unrealized_pnl_calculator);
        aos::impl::NetPositionStrategyDefault<Price, Qty> strategy(
            realized_pnl_storage, un_realized_pnl_storage);

        aos::impl::PositionStorageByPair<
            Price, Qty, aos::impl::NetPositionDefault<Price, Qty>>
            position_storage_by_pair([&]() {
                return aos::impl::NetPositionDefault<Price, Qty>(strategy);
            });

        common::MemoryPoolThreadSafety<aoe::bybit::ExecutionEventSpotBuyDefault<
            common::MemoryPoolThreadSafety, PositionT>>
            pool_event(1);
        aoe::bybit::impl::ExecutionWatcherDefault<
            common::MemoryPoolThreadSafety, PositionT>
            watcher(position_storage_by_pair);
        auto* ptr = pool_event.Allocate();
        ptr->SetExecPrice(100);
        ptr->SetExecQty(10);
        ptr->SetMemoryPool(&pool_event);
        watcher.OnEvent(ptr);
    }
    fmtlog::poll();
    return 0;
}
