#pragma once
#include "aos/position_strategy/i_position_strategy.h"

namespace aos {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class NetPositionStrategy : public IPositionStrategy<Price, Qty, MemoryPool> {
  public:
    void Add(Price& avg_price, Qty& net_qty, Price price,
             Qty qty) const override {
        const Price total_value  = net_qty * avg_price + qty * price;
        net_qty                 += qty;
        avg_price                = total_value / net_qty;
    }

    void Remove(Price& avg_price, Qty& net_qty, Price price,
                Qty qty) const override {
        net_qty   -= qty;
        avg_price  = (net_qty != 0)
                         ? (avg_price * (net_qty + qty) - price * qty) / net_qty
                         : 0;
    }
};

template <typename PositionStrategyT, typename Price, typename Qty,
          template <typename> typename MemoryPool>
class PositionStrategyBuilder {
    MemoryPool<PositionStrategyT>& pool_;

  public:
    explicit PositionStrategyBuilder(MemoryPool<PositionStrategyT>& pool)
        : pool_(pool) {}

    auto Build() {
        auto* obj = pool_.Allocate();
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<PositionStrategyT>(obj);
    }
};

template <typename NetPositionStrategyT, typename NetPositionStrategyBuilderT,
          typename Price, typename Qty, template <typename> typename MemoryPool>
class PositionStrategyContainer {
    MemoryPool<NetPositionStrategyT> pool_;

  public:
    explicit PositionStrategyContainer(size_t size) : pool_(size) {}

    auto Build() {
        NetPositionStrategyBuilderT builder(pool_);
        auto strategy = builder.Build();
        return strategy;
    }
};

};  // namespace impl
};  // namespace aos