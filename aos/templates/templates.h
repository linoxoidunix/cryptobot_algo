#pragma once

#include "aos/aos.h"

namespace aos::types {

template <typename PriceT = double, typename QtyT = double,
          typename UidT = std::size_t>
struct BaseTypes {
    using Price = PriceT;
    using Qty   = QtyT;
    using Uid   = UidT;
};

template <typename Base, typename UnRealizedStorageT>
struct PnlStorageTypes {
    using RealizedPnlStorageContainerT = aos::impl::RealizedPnlStorageContainer<
        typename Base::Price, typename Base::Qty,
        common::MemoryPoolNotThreadSafety>;

    using UnRealizedPnlCalculatorT =
        aos::impl::UnRealizedPnlCalculator<typename Base::Price,
                                           typename Base::Qty,
                                           common::MemoryPoolNotThreadSafety>;

    using UnRealizedPnlCalculatorContainerT =
        aos::impl::UnRealizedPnlCalculatorContainer<
            typename Base::Price, typename Base::Qty,
            common::MemoryPoolNotThreadSafety, UnRealizedPnlCalculatorT>;

    using UnrealizedPnlStorageContainerT =
        aos::impl::UnRealizedPnlStorageContainer<
            typename Base::Price, typename Base::Qty,
            common::MemoryPoolNotThreadSafety,
            UnRealizedPnlCalculatorContainerT, UnRealizedStorageT>;
};

// Net-режим
template <typename Price = double, typename Qty = double>
struct NetPositionTypes {
    using Base = BaseTypes<Price, Qty>;
    using Storage =
        PnlStorageTypes<Base,
                        aos::impl::NetUnRealizedPnlStorage<
                            Price, Qty, common::MemoryPoolNotThreadSafety>>;

    using Position =
        aos::impl::NetPosition<typename Base::Price, typename Base::Qty,
                               common::MemoryPoolNotThreadSafety>;
    using PositionStrategy =
        aos::impl::NetPositionStrategy<Price, Qty,
                                       common::MemoryPoolNotThreadSafety>;
    using Strategy =
        aos::impl::NetPositionStrategy<typename Base::Price, typename Base::Qty,
                                       common::MemoryPoolNotThreadSafety>;

    using StrategyBuilder =
        aos::impl::PositionStrategyBuilder<Strategy, typename Base::Price,
                                           typename Base::Qty,
                                           common::MemoryPoolNotThreadSafety>;

    using PositionStrategyContainerT = aos::impl::PositionStrategyContainer<
        Strategy, StrategyBuilder, typename Base::Price, typename Base::Qty,
        common::MemoryPoolNotThreadSafety,
        typename Storage::RealizedPnlStorageContainerT,
        typename Storage::UnrealizedPnlStorageContainerT>;

    using PositionStorageByPairT = aos::impl::PositionStorageContainer<
        Price, Qty, common::MemoryPoolNotThreadSafety,
        aos::impl::PositionStorageByPair<Price, Qty,
                                         common::MemoryPoolNotThreadSafety,
                                         Position, PositionStrategy>,
        PositionStrategy>;
};

// Hedge-режим
template <typename Price = double, typename Qty = double>
struct HedgePositionTypes {
    using Base = BaseTypes<Price, Qty>;
    using Storage =
        PnlStorageTypes<Base,
                        aos::impl::HedgedUnRealizedPnlStorage<
                            Price, Qty, common::MemoryPoolNotThreadSafety>>;

    using Position =
        aos::impl::HedgePosition<typename Base::Price, typename Base::Qty,
                                 common::MemoryPoolNotThreadSafety>;
    using PositionStrategy =
        aos::impl::HedgedPositionStrategy<Price, Qty,
                                          common::MemoryPoolNotThreadSafety>;

    using Strategy =
        aos::impl::HedgedPositionStrategy<typename Base::Price,
                                          typename Base::Qty,
                                          common::MemoryPoolNotThreadSafety>;

    using StrategyBuilder =
        aos::impl::PositionStrategyBuilder<Strategy, typename Base::Price,
                                           typename Base::Qty,
                                           common::MemoryPoolNotThreadSafety>;

    using PositionStrategyContainerT = aos::impl::PositionStrategyContainer<
        Strategy, StrategyBuilder, typename Base::Price, typename Base::Qty,
        common::MemoryPoolNotThreadSafety,
        typename Storage::RealizedPnlStorageContainerT,
        typename Storage::UnrealizedPnlStorageContainerT>;

    using PositionStorageByPairT = aos::impl::PositionStorageContainer<
        Price, Qty, common::MemoryPoolNotThreadSafety,
        aos::impl::PositionStorageByPair<Price, Qty,
                                         common::MemoryPoolNotThreadSafety,
                                         Position, PositionStrategy>,
        PositionStrategy>;
};
};  // namespace aos::types
