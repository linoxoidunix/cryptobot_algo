// Copyright 2025 Denis Evlanov

#include <iostream>

#include "aos/common/mem_pool.h"
#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "boost/intrusive_ptr.hpp"

int main() {
    try {
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
    } catch (...) {
        loge("Unknown exception caught");
    }
    fmtlog::poll();
    return 0;
}
