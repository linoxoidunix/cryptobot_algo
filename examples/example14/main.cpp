#include <iostream>

#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "aos/common/mem_pool.h"
#include "boost/intrusive_ptr.hpp"

int main() {
    {
        using Price = double;
        using Qty   = double;
        using Uid   = size_t;
        using UnRealizedPnlCalculatorContainerT =
            aos::impl::UnRealizedPnlCalculatorContainer<
                Price, Qty, common::MemoryPoolNotThreadSafety,
                aos::impl::UnRealizedPnlCalculator<
                    Price, Qty, common::MemoryPoolNotThreadSafety>>;
        aos::impl::UnRealizedPnlStorageContainer<
            Price, Qty, common::MemoryPoolNotThreadSafety,
            UnRealizedPnlCalculatorContainerT,
            aos::impl::NetUnRealizedPnlStorage<
                Price, Qty, common::MemoryPoolNotThreadSafety>>
            container(1);
        auto storage = container.Build();
    }
    fmtlog::poll();
    return 0;
}