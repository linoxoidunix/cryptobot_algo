#pragma once
#include "aos/pnl/unrealized_calculator/i_pnl_unrealized_calculator.h"
#include "boost/intrusive_ptr.hpp"

namespace aos {
namespace impl {

template <typename Price, typename Qty>
class UnRealizedPnlCalculatorDefault
    : public UnRealizedPnlCalculatorInterface<Price, Qty> {
    using UnRealizedPnl =
        UnRealizedPnlCalculatorInterface<Price, Qty>::UnRealizedPnl;

  public:
    ~UnRealizedPnlCalculatorDefault() override {};
    // Реализация метода Calculate для интерфейса
    UnRealizedPnl Calculate(Price avg_price, Qty net_qty, Price bid,
                            Price ask) override {
        using UnRealizedPnl      = decltype(avg_price * net_qty);

        const bool is_long       = net_qty > Qty{};
        const Qty abs_qty        = is_long ? net_qty : -net_qty;

        const Price market_price = is_long ? bid : ask;
        const Price price_diff =
            is_long ? (market_price - avg_price) : (avg_price - market_price);

        return price_diff * abs_qty;
    };
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class UnRealizedPnlCalculator
    : public IUnRealizedPnlCalculator<Price, Qty, MemoryPool> {
    using UnRealizedPnl =
        IUnRealizedPnlCalculator<Price, Qty, MemoryPool>::UnRealizedPnl;

  public:
    // Реализация метода Calculate для интерфейса
    UnRealizedPnl Calculate(Price avg_price, Qty net_qty, Price bid,
                            Price ask) override {
        using UnRealizedPnl      = decltype(avg_price * net_qty);

        const bool is_long       = net_qty > Qty{};
        const Qty abs_qty        = is_long ? net_qty : -net_qty;

        const Price market_price = is_long ? bid : ask;
        const Price price_diff =
            is_long ? (market_price - avg_price) : (avg_price - market_price);

        return price_diff * abs_qty;
    };
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename UnRealizedPnlCalculatorT>
class UnRealizedPnlCalculatorBuilder {
    MemoryPool<UnRealizedPnlCalculatorT>& pool_;

  public:
    explicit UnRealizedPnlCalculatorBuilder(
        MemoryPool<UnRealizedPnlCalculatorT>& pool)
        : pool_(pool) {}

    auto Build() {
        auto* obj = pool_.Allocate();
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<
            IUnRealizedPnlCalculator<Price, Qty, MemoryPool>>(obj);
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename UnRealizedPnlCalculatorT>
class UnRealizedPnlCalculatorContainer {
    MemoryPool<UnRealizedPnlCalculator<Price, Qty, MemoryPool>> pool_;
    using Builder = UnRealizedPnlCalculatorBuilder<Price, Qty, MemoryPool,
                                                   UnRealizedPnlCalculatorT>;

  public:
    explicit UnRealizedPnlCalculatorContainer(size_t size) : pool_(size) {}

    auto Build() {
        Builder builder(pool_);
        return builder.Build();
    }
};
}  // namespace impl
}  // namespace aos