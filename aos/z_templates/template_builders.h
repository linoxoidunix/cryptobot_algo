#pragma once

#include "aos/aos.h"
#include "aot/common/mem_pool.h"

namespace aos::types {

template <typename PriceT = double, typename QtyT = double,
          typename UidT = std::size_t>
struct BaseTypes {
    using Price = PriceT;
    using Qty   = QtyT;
    using Uid   = UidT;
};

template <typename BaseTypesT, typename UnRealizedPnlStorageT,
          typename PositionStrategyT, typename PositionT>
class ComponentsBuilder {
    using Price = typename BaseTypesT::Price;
    using Qty   = typename BaseTypesT::Qty;

    using RealizedPnlStorage =
        aos::impl::RealizedPnlStorage<Price, Qty,
                                      common::MemoryPoolNotThreadSafety>;

    aos::impl::RealizedPnlStorageContainer<
        Price, Qty, common::MemoryPoolNotThreadSafety, RealizedPnlStorage>
        realized_pnl_storage_container_{1};

    common::MemoryPoolNotThreadSafety<UnRealizedPnlStorageT>
        unrealized_pnl_storage_pool_{1};

    using UnRealizedPnlCalculatorT =
        aos::impl::UnRealizedPnlCalculator<Price, Qty,
                                           common::MemoryPoolNotThreadSafety>;

    aos::impl::UnRealizedPnlCalculatorContainer<
        Price, Qty, common::MemoryPoolNotThreadSafety, UnRealizedPnlCalculatorT>
        unrealized_pnl_calc_container_{1};

    common::MemoryPoolNotThreadSafety<PositionStrategyT>
        position_strategy_pool_{1};

    using PositionStorageByPairT =
        aos::impl::PositionStorageByPair<Price, Qty,
                                         common::MemoryPoolNotThreadSafety,
                                         PositionT, PositionStrategyT>;

    aos::impl::PositionStorageContainer<
        Price, Qty, common::MemoryPoolNotThreadSafety, PositionStorageByPairT,
        PositionT, PositionStrategyT>
        position_storage_container_{1};

  public:
    auto Build() {
        auto realized_pnl_storage_ptr = realized_pnl_storage_container_.Build();
        auto unrealized_pnl_calc_ptr  = unrealized_pnl_calc_container_.Build();

        auto unrealized_pnl_storage_ptr =
            aos::impl::UnRealizedPnlStorageBuilder<
                Price, Qty, common::MemoryPoolNotThreadSafety,
                UnRealizedPnlStorageT>(unrealized_pnl_storage_pool_)
                .SetPnlUnrealizedCalculator(unrealized_pnl_calc_ptr)
                .Build();

        auto position_strategy_ptr =
            aos::impl::PositionStrategyBuilder<
                PositionStrategyT, Price, Qty,
                common::MemoryPoolNotThreadSafety>(position_strategy_pool_)
                .SetPnlRealizedStorage(realized_pnl_storage_ptr)
                .SetPnlUnRealizedStorage(unrealized_pnl_storage_ptr)
                .Build();

        auto position_storage_ptr =
            position_storage_container_
                .SetPositionStrategy(position_strategy_ptr)
                .Build();

        return std::make_tuple(realized_pnl_storage_ptr,
                               unrealized_pnl_storage_ptr,
                               unrealized_pnl_calc_ptr, position_strategy_ptr,
                               position_storage_ptr);
    }
};

template <typename BaseTypesT = aos::types::BaseTypes<>>
using NetComponents = ComponentsBuilder<
    BaseTypesT,
    aos::impl::NetUnRealizedPnlStorage<typename BaseTypesT::Price,
                                       typename BaseTypesT::Qty,
                                       common::MemoryPoolNotThreadSafety>,
    aos::impl::NetPositionStrategy<typename BaseTypesT::Price,
                                   typename BaseTypesT::Qty,
                                   common::MemoryPoolNotThreadSafety>,
    aos::impl::NetPosition<typename BaseTypesT::Price, typename BaseTypesT::Qty,
                           common::MemoryPoolNotThreadSafety>>;

template <typename BaseTypesT = aos::types::BaseTypes<>>
using HedgedComponents =
    ComponentsBuilder<BaseTypesT,
                      aos::impl::HedgedUnRealizedPnlStorage<
                          typename BaseTypesT::Price, typename BaseTypesT::Qty,
                          common::MemoryPoolNotThreadSafety>,
                      aos::impl::HedgedPositionStrategy<
                          typename BaseTypesT::Price, typename BaseTypesT::Qty,
                          common::MemoryPoolNotThreadSafety>,
                      aos::impl::HedgePosition<
                          typename BaseTypesT::Price, typename BaseTypesT::Qty,
                          common::MemoryPoolNotThreadSafety>>;

};  // namespace aos::types

// #pragma once

// #include "aos/aos.h"
// #include "aot/common/mem_pool.h"

// namespace aos::types {
// template <typename PriceT = double, typename QtyT = double,
//           typename UidT = std::size_t>
// struct BaseTypes {
//     using Price = PriceT;
//     using Qty   = QtyT;
//     using Uid   = UidT;
// };

// template <typename BaseTypesT = aos::types::BaseTypes<>>
// class NetComponents {
//     using Price = typename BaseTypesT::Price;
//     using Qty   = typename BaseTypesT::Qty;
//     using Uid   = typename BaseTypesT::Uid;
//     using RealizedPnlStorage =
//         aos::impl::RealizedPnlStorage<Price, Qty,
//                                       common::MemoryPoolNotThreadSafety>;
//     using UnRealizedPnlStorage =
//         aos::impl::NetUnRealizedPnlStorage<Price, Qty,
//                                            common::MemoryPoolNotThreadSafety>;
//     aos::impl::RealizedPnlStorageContainer<
//         Price, Qty, common::MemoryPoolNotThreadSafety, RealizedPnlStorage>
//         realized_pnl_storage_container_{1};
//     common::MemoryPoolNotThreadSafety<UnRealizedPnlStorage>
//         unrealized_pnl_storage_pool_{1};
//     using UnRealizedPnlCalculatorT =
//         aos::impl::UnRealizedPnlCalculator<Price, Qty,
//                                            common::MemoryPoolNotThreadSafety>;
//     aos::impl::UnRealizedPnlCalculatorContainer<
//         Price, Qty, common::MemoryPoolNotThreadSafety,
//         UnRealizedPnlCalculatorT> unrealized_pnl_calc_container_{1};
//     using PositionStrategy =
//         aos::impl::NetPositionStrategy<Price, Qty,
//                                        common::MemoryPoolNotThreadSafety>;
//     common::MemoryPoolNotThreadSafety<PositionStrategy>
//     position_strategy_pool_{
//         1};
//     using PositionT =
//         aos::impl::NetPosition<Price, Qty,
//         common::MemoryPoolNotThreadSafety>;
//     using PositionStorageByPairT =
//         aos::impl::PositionStorageByPair<Price, Qty,
//                                          common::MemoryPoolNotThreadSafety,
//                                          PositionT, PositionStrategy>;
//     aos::impl::PositionStorageContainer<
//         Price, Qty, common::MemoryPoolNotThreadSafety,
//         PositionStorageByPairT, PositionT, PositionStrategy>
//         position_storage_container_{1};

//   public:
//     auto Build() {
//         auto realized_pnl_storage_ptr_ =
//             realized_pnl_storage_container_.Build();

//         auto unrealized_pnl_calc_ptr =
//         unrealized_pnl_calc_container_.Build();

//         auto unrealized_pnl_storage_ptr_ =
//             aos::impl::UnRealizedPnlStorageBuilder<
//                 Price, Qty, common::MemoryPoolNotThreadSafety,
//                 UnRealizedPnlStorage>(unrealized_pnl_storage_pool_)
//                 .SetPnlUnrealizedCalculator(unrealized_pnl_calc_ptr)
//                 .Build();
//         auto position_strategy_ptr =
//             aos::impl::PositionStrategyBuilder<
//                 PositionStrategy, Price, Qty,
//                 common::MemoryPoolNotThreadSafety>(position_strategy_pool_)
//                 .SetPnlRealizedStorage(realized_pnl_storage_ptr_)
//                 .SetPnlUnRealizedStorage(unrealized_pnl_storage_ptr_)
//                 .Build();
//         auto position_storage_ptr =
//             position_storage_container_
//                 .SetPositionStrategy(position_strategy_ptr)
//                 .Build();
//         return std::make_tuple(realized_pnl_storage_ptr_,
//                                unrealized_pnl_storage_ptr_,
//                                unrealized_pnl_calc_ptr,
//                                position_strategy_ptr, position_storage_ptr);
//     }
// };

// template <typename BaseTypesT = aos::types::BaseTypes<>>
// class HedgedComponents {
//     using Price = typename BaseTypesT::Price;
//     using Qty   = typename BaseTypesT::Qty;
//     using Uid   = typename BaseTypesT::Uid;
//     using RealizedPnlStorage =
//         aos::impl::RealizedPnlStorage<Price, Qty,
//                                       common::MemoryPoolNotThreadSafety>;
//     using UnRealizedPnlStorage = aos::impl::HedgedUnRealizedPnlStorage<
//         Price, Qty, common::MemoryPoolNotThreadSafety>;
//     aos::impl::RealizedPnlStorageContainer<
//         Price, Qty, common::MemoryPoolNotThreadSafety, RealizedPnlStorage>
//         realized_pnl_storage_container_{1};
//     common::MemoryPoolNotThreadSafety<UnRealizedPnlStorage>
//         unrealized_pnl_storage_pool_{1};
//     using UnRealizedPnlCalculatorT =
//         aos::impl::UnRealizedPnlCalculator<Price, Qty,
//                                            common::MemoryPoolNotThreadSafety>;
//     aos::impl::UnRealizedPnlCalculatorContainer<
//         Price, Qty, common::MemoryPoolNotThreadSafety,
//         UnRealizedPnlCalculatorT> unrealized_pnl_calc_container_{1};

//     using PositionStrategy =
//         aos::impl::HedgedPositionStrategy<Price, Qty,
//                                           common::MemoryPoolNotThreadSafety>;
//     common::MemoryPoolNotThreadSafety<PositionStrategy>
//     position_strategy_pool_{
//         1};
//     using PositionT =
//         aos::impl::HedgePosition<Price, Qty,
//         common::MemoryPoolNotThreadSafety>;
//     using PositionStorageByPairT =
//         aos::impl::PositionStorageByPair<Price, Qty,
//                                          common::MemoryPoolNotThreadSafety,
//                                          PositionT, PositionStrategy>;
//     aos::impl::PositionStorageContainer<
//         Price, Qty, common::MemoryPoolNotThreadSafety,
//         PositionStorageByPairT, PositionT, PositionStrategy>
//         position_storage_container_{1};

//   public:
//     auto Build() {
//         auto realized_pnl_storage_ptr_ =
//             realized_pnl_storage_container_.Build();

//         auto unrealized_pnl_calc_ptr =
//         unrealized_pnl_calc_container_.Build();

//         auto unrealized_pnl_storage_ptr_ =
//             aos::impl::UnRealizedPnlStorageBuilder<
//                 Price, Qty, common::MemoryPoolNotThreadSafety,
//                 UnRealizedPnlStorage>(unrealized_pnl_storage_pool_)
//                 .SetPnlUnrealizedCalculator(unrealized_pnl_calc_ptr)
//                 .Build();
//         auto position_strategy_ptr =
//             aos::impl::PositionStrategyBuilder<
//                 PositionStrategy, Price, Qty,
//                 common::MemoryPoolNotThreadSafety>(position_strategy_pool_)
//                 .SetPnlRealizedStorage(realized_pnl_storage_ptr_)
//                 .SetPnlUnRealizedStorage(unrealized_pnl_storage_ptr_)
//                 .Build();
//         auto position_storage_ptr =
//             position_storage_container_
//                 .SetPositionStrategy(position_strategy_ptr)
//                 .Build();
//         return std::make_tuple(realized_pnl_storage_ptr_,
//                                unrealized_pnl_storage_ptr_,
//                                unrealized_pnl_calc_ptr,
//                                position_strategy_ptr, position_storage_ptr);
//     }
// };

// };  // namespace aos::types