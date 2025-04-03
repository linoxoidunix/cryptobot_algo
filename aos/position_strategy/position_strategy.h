#pragma once
#include "aos/pnl/realized_calculator/pnl_realized_calculator.h"
#include "aos/pnl/realized_storage/i_pnl_realized_storage.h"
#include "aos/position_strategy/i_position_strategy.h"
namespace aos {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class NetPositionStrategy
    : public INetPositionStrategy<Price, Qty, MemoryPool> {
    boost::intrusive_ptr<IPnlRealizedStorage<Price, Qty, MemoryPool>>
        pnl_realized_storage_;

  public:
    NetPositionStrategy(
        boost::intrusive_ptr<IPnlRealizedStorage<Price, Qty, MemoryPool>>
            pnl_realized_storage)
        : pnl_realized_storage_(pnl_realized_storage) {}
    void Add(common::ExchangeId exchange_id, common::TradingPair trading_pair,
             Price& avg_price, Qty& net_qty, Price price,
             Qty qty) const override {
        const Price total_value  = net_qty * avg_price + qty * price;
        net_qty                 += qty;
        avg_price                = total_value / net_qty;
    }

    INetPositionStrategy<Price, Qty, MemoryPool>::RealizedPnl Remove(
        common::ExchangeId exchange_id, common::TradingPair trading_pair,
        Price& avg_price, Qty& net_qty, Price price, Qty qty) const override {
        auto realized_pnl = RealizedPnlCalculator<Price, Qty>::Calculate(
            avg_price, net_qty, price, qty);
        net_qty   -= qty;
        avg_price  = (net_qty != 0)
                         ? (avg_price * (net_qty + qty) - price * qty) / net_qty
                         : 0;
        pnl_realized_storage_->Add(exchange_id, trading_pair, realized_pnl);
        return realized_pnl;
    }
    ~NetPositionStrategy() override {}
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class HedgedPositionStrategy
    : public IHedgePositionStrategy<Price, Qty, MemoryPool> {
    boost::intrusive_ptr<IPnlRealizedStorage<Price, Qty, MemoryPool>>
        pnl_realized_storage_;

  public:
    HedgedPositionStrategy(
        boost::intrusive_ptr<IPnlRealizedStorage<Price, Qty, MemoryPool>>
            pnl_realized_storage)
        : pnl_realized_storage_(pnl_realized_storage) {}

    void Add(common::ExchangeId exchange_id, common::TradingPair trading_pair,
             Price (&avg_price)[2], Qty (&net_qty)[2], Price price,
             Qty qty) const override {
        int idx = (qty > 0) ? 1 : 0;  // Если цена положительная, то long (1),
                                      // если отрицательная — short (0)
        const Price total_value  = net_qty[idx] * avg_price[idx] + qty * price;
        net_qty[idx]            += qty;
        avg_price[idx]           = total_value / net_qty[idx];
    }

    IHedgePositionStrategy<Price, Qty, MemoryPool>::RealizedPnl Remove(
        common::ExchangeId exchange_id, common::TradingPair trading_pair,
        Price (&avg_price)[2], Qty (&net_qty)[2], Price price,
        Qty qty) const override {
        const int idx     = (qty > 0);
        auto realized_pnl = RealizedPnlCalculator<Price, Qty>::Calculate(
            avg_price[idx], net_qty[idx], price, qty);
        pnl_realized_storage_->Add(exchange_id, trading_pair, realized_pnl);
        Qty abs_qty          = std::abs(qty);
        Qty removable        = std::min(std::abs(net_qty[idx]), abs_qty);
        removable            = (qty < 0) ? -removable : removable;
        const Qty remaining  = qty - removable;
        net_qty[idx]        -= removable;
        avg_price[idx] =
            (net_qty[idx] > 0)
                ? (avg_price[idx] * net_qty[idx] - realized_pnl) / net_qty[idx]
                : 0;
        net_qty[1 - idx] += remaining;
        const Price total_value_new_side =
            avg_price[1 - idx] * net_qty[1 - idx] + price * remaining;
        avg_price[1 - idx] = (remaining)
                                 ? (total_value_new_side / net_qty[1 - idx])
                                 : avg_price[1 - idx];
        return realized_pnl;
    }

    ~HedgedPositionStrategy() override {}
};

template <typename PositionStrategyT, typename Price, typename Qty,
          template <typename> typename MemoryPool>
class PositionStrategyBuilder {
    MemoryPool<PositionStrategyT>& pool_;

  public:
    using Strategy = PositionStrategyT;
    explicit PositionStrategyBuilder(MemoryPool<PositionStrategyT>& pool)
        : pool_(pool) {}

    template <typename... Args>
    auto Build(Args&&... args) {
        auto* obj = pool_.Allocate(std::forward<Args>(args)...);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<PositionStrategyT>(obj);
    }
};

template <typename NetPositionStrategyT, typename NetPositionStrategyBuilderT,
          typename Price, typename Qty, template <typename> typename MemoryPool>
class PositionStrategyContainer {
    MemoryPool<NetPositionStrategyT> pool_;
    using Builder = NetPositionStrategyBuilderT;
    boost::intrusive_ptr<IPnlRealizedStorage<Price, Qty, MemoryPool>>
        pnl_realized_storage_;

  public:
    explicit PositionStrategyContainer(
        size_t size,
        boost::intrusive_ptr<IPnlRealizedStorage<Price, Qty, MemoryPool>>
            pnl_realized_storage)
        : pool_(size), pnl_realized_storage_(pnl_realized_storage) {}

    auto Build() {
        NetPositionStrategyBuilderT builder(pool_);
        auto strategy = builder.Build(pnl_realized_storage_);
        return strategy;
    }
};

};  // namespace impl
};  // namespace aos