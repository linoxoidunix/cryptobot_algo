#pragma once
#include <list>

// #include "aos/exchange/i_exchange.h"
#include "aos/exchange/i_exchange.h"
// #include "aot/common/mem_pool.h"
#include "boost/asio/thread_pool.hpp"

namespace aos {

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class IOrderUpdater;

namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class BinanceExchange : public aos::IExchange<Price, Qty, MemoryPool, Uid> {
    boost::asio::thread_pool& pool_;
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    std::list<boost::intrusive_ptr<IOrderUpdater<Price, Qty, MemoryPool, Uid>>>
        subscribers_;

  public:
    BinanceExchange(boost::asio::thread_pool& pool)
        : pool_(pool), strand_(boost::asio::make_strand(pool)) {}
    void SubmitOrder(common::TradingPair trading_pair, Price price, Qty qty,
                     common::Side side, TimeInForce tif, Uid uid) override {
        // Implementation for submitting order to Binance exchange
        logi("Submit order on binance exchange");
        for (auto it : subscribers_) {
            it->UpdateStatusOrder(uid, OrderStatus::Filled);
        }
    }
    void CancelOrder(Uid uid) override {
        // Implementation for canceling order in Binance exchange
        for (auto it : subscribers_) {
            it->UpdateStatusOrder(uid, OrderStatus::CanceledByExchange);
        }
    }

    void Subscribe(
        boost::intrusive_ptr<aos::IOrderUpdater<Price, Qty, MemoryPool, Uid>>
            subscriber) override {
        subscribers_.push_back(subscriber);
    }
    ~BinanceExchange() override {
        // subscribers_.clear();
        logi("BinanceExchange dtor");
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class BinanceExchangeBuilder {
    MemoryPool<BinanceExchange<Price, Qty, MemoryPool, Uid>>& pool_;
    boost::asio::thread_pool& thread_pool_;

  public:
    BinanceExchangeBuilder(
        MemoryPool<BinanceExchange<Price, Qty, MemoryPool, Uid>>& pool,
        boost::asio::thread_pool& thread_pool)
        : pool_(pool), thread_pool_(thread_pool) {}

    boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>> Build() {
        auto* obj = pool_.Allocate(thread_pool_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>>(
            obj);
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class BinanceExchangeContainer {
    MemoryPool<BinanceExchange<Price, Qty, MemoryPool, Uid>> pool_;
    boost::asio::thread_pool& thread_pool_;

  public:
    BinanceExchangeContainer(size_t size, boost::asio::thread_pool& thread_pool)
        : pool_(size), thread_pool_(thread_pool) {}

    boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>> Build() {
        BinanceExchangeBuilder<Price, Qty, MemoryPool, Uid> builder(
            pool_, thread_pool_);
        return builder.Build();
    }
};  // namespace impl
};  // namespace impl
};  // namespace aos