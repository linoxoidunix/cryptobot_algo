#pragma once
#include <functional>
#include <unordered_map>

#include "aos/exchange_registry/exchange_registry.h"
#include "aos/exchange_registry/i_exchange_registry.h"
#include "aos/order_manager/order_submitter/i_order_submitter.h"
#include "aot/common/mem_pool.h"

namespace aos {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class OrderSubmitter
    : public IOrderSubmitter<Price, Qty, common::MemoryPoolNotThreadSafety,
                             Uid> {
    boost::intrusive_ptr<IExchangeRegistry<Price, Qty, MemoryPool, Uid>>
        exchange_registry_;

  public:
    explicit OrderSubmitter(
        boost::intrusive_ptr<IExchangeRegistry<Price, Qty, MemoryPool, Uid>>
            exchange_registry)
        : exchange_registry_(exchange_registry) {};
    ~OrderSubmitter() override { logi("OrderSubmitter dtor"); };

    void SubmitOrder(common::ExchangeId exchange_id,
                     common::TradingPair trading_pair, Price price, Qty qty,
                     common::Side side, TimeInForce tif, OrderRouting routing,
                     Uid uid) override {
        auto [status, exchange] =
            exchange_registry_->GetExchange(routing, exchange_id);
        if (!status) {
            loge("No exchange found for routing: {}",
                 static_cast<int>(routing));
            return;
        }
        exchange->SubmitOrder(trading_pair, price, qty, side, tif, uid);
    }
    void SubmitCancelOrder(common::ExchangeId exchange_id, OrderRouting routing,
                           Uid uid) override {
        auto [status, exchange] =
            exchange_registry_->GetExchange(routing, exchange_id);
        if (!status) {
            loge("No exchange found for routing: {}",
                 static_cast<int>(routing));
            return;
        }
        exchange->CancelOrder(uid);
    }
};

template <typename OrderSubmitterT, typename Price, typename Qty,
          template <typename> typename MemoryPool, typename Uid>
class OrderSubmitterBuilder {
    MemoryPool<OrderSubmitterT>& pool_;
    boost::intrusive_ptr<IExchangeRegistry<Price, Qty, MemoryPool, Uid>>
        exchange_registry_;

  public:
    explicit OrderSubmitterBuilder(MemoryPool<OrderSubmitterT>& pool)
        : pool_(pool) {}

    OrderSubmitterBuilder& SetExchangeRegistry(auto exchange_registry) {
        exchange_registry_ = exchange_registry;
        return *this;
    }

    auto Build() {
        auto* obj = pool_.Allocate(exchange_registry_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<
            IOrderSubmitter<Price, Qty, MemoryPool, Uid>>(obj);
    }
};

template <typename OrderSubmitterT, typename OrderSubmitterBuilderT,
          typename ExchangeRegistryT, typename ExchangeRegistryBuilderT,
          typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class OrderSubmitterContainer {
    MemoryPool<OrderSubmitterT> pool_;
    MemoryPool<ExchangeRegistryT> pool_exchange_registry_;

  public:
    explicit OrderSubmitterContainer(size_t size)
        : pool_(size), pool_exchange_registry_(size) {}

    auto Build() {
        ExchangeRegistryBuilderT exchange_registry_builder(
            pool_exchange_registry_);
        auto exch_reg_ptr_ = exchange_registry_builder.Build();

        OrderSubmitterBuilderT builder(pool_);
        builder.SetExchangeRegistry(exch_reg_ptr_);
        auto order_submitter = builder.Build();

        return std::make_tuple(order_submitter, exch_reg_ptr_);
    }
};
};  // namespace impl
};  // namespace aos