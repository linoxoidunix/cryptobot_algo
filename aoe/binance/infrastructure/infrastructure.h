#pragma once
#include <memory>
#include <thread>
#include <unordered_map>

#include "aoe/binance/infrastructure/i_infrastructure.h"
#include "aoe/binance/order_book_sync/order_book_sync.h"
#include "aoe/binance/response_queue_listener/json/ws/diff_response/listener.h"
#include "aoe/binance/session/web_socket/web_socket.h"
#include "aoe/binance/subscription_builder/subscription_builder.h"
#include "aos/order_book/order_book.h"
#include "aos/order_book_level/order_book_level.h"
#include "aos/order_book_subscriber/order_book_subscribers.h"
#include "aos/trading_pair/trading_pair.h"
#include "concurrentqueue.h"

namespace aoe {
namespace binance {
namespace impl {

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool, typename OrderBookSyncT,
          typename ListenerT, typename SessionRWType>
struct OrderBookInfraContext {
    moodycamel::ConcurrentQueue<std::vector<char>> queue;
    boost::asio::io_context ioc;

    std::unique_ptr<aos::OrderBookEventListener<
        Price, Qty, MemoryPoolThreadSafety,
        std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>>
        order_book;

    std::unique_ptr<aos::OrderBookNotifier<Price, Qty, MemoryPoolThreadSafety>>
        notifier;

    std::unique_ptr<OrderBookSyncT> sync;

    std::optional<std::unique_ptr<aos::BestBidNotifierInterface<Price, Qty>>>
        best_bid_notifier;
    std::optional<std::unique_ptr<aos::BestAskNotifierInterface<Price, Qty>>>
        best_ask_notifier;
    std::optional<std::unique_ptr<aos::BestBidPriceNotifierInterface<Price>>>
        best_bid_price_notifier;
    std::optional<std::unique_ptr<aos::BestAskPriceNotifierInterface<Price>>>
        best_ask_price_notifier;

    std::unique_ptr<ListenerT> listener;
    std::unique_ptr<SessionRWType> session;
    std::unique_ptr<aoe::SubscriptionBuilderInterface>
        diff_subscription_builder;

    std::jthread thread;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        work_guard_{boost::asio::make_work_guard(ioc)};
};

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool, typename OrderBookSyncT,
          typename ListenerT, typename SessionRWType,
          typename DiffSubscriptionBuilderT, typename UpdateSpeedType,
          UpdateSpeedType UpdateSpeedT, auto NotifierFlagsT>
class InfrastructureImpl : public InfrastructureInterface,
                           public InfrastructureNotifierInterface<Price, Qty> {
    using ContextT =
        OrderBookInfraContext<Price, Qty, MemoryPoolThreadSafety, MemoryPool,
                              OrderBookSyncT, ListenerT, SessionRWType>;

  public:
    explicit InfrastructureImpl(boost::asio::thread_pool& pool) : pool_(pool) {}

    void Register(aos::TradingPair trading_pair) override {
        if (contexts_.contains(trading_pair)) return;

        auto context_ptr   = std::make_shared<ContextT>();
        auto& context      = *context_ptr;
        auto& queue        = context.queue;

        context.order_book = std::make_unique<aos::OrderBookEventListener<
            Price, Qty, MemoryPoolThreadSafety,
            std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>>(pool_,
                                                                          1000);

        context.notifier   = std::make_unique<
              aos::OrderBookNotifier<Price, Qty, MemoryPoolThreadSafety>>(
            *context.order_book);

        context.sync =
            std::make_unique<OrderBookSyncT>(pool_, *context.notifier);

        if constexpr (NotifierFlagsT.best_bid_enabled) {
            context.best_bid_notifier =
                std::make_unique<aos::impl::BestBidNotifier<Price, Qty>>(
                    pool_, *context.order_book);
        }

        if constexpr (NotifierFlagsT.best_ask_enabled) {
            context.best_ask_notifier =
                std::make_unique<aos::impl::BestAskNotifier<Price, Qty>>(
                    pool_, *context.order_book);
        }

        if constexpr (NotifierFlagsT.best_bid_price_enabled) {
            context.best_bid_price_notifier =
                std::make_unique<aos::impl::BestBidPriceNotifier<Price, Qty>>(
                    pool_, *context.order_book);
        }

        if constexpr (NotifierFlagsT.best_ask_price_enabled) {
            context.best_ask_price_notifier =
                std::make_unique<aos::impl::BestAskPriceNotifier<Price, Qty>>(
                    pool_, *context.order_book);
        }

        context.listener =
            std::make_unique<ListenerT>(pool_, queue, *context.sync);

        context.thread =
            std::jthread([ioc_ptr = &context.ioc]() { ioc_ptr->run(); });

        context.session = std::make_unique<SessionRWType>(context.ioc, queue,
                                                          *context.listener);

        context.diff_subscription_builder =
            std::make_unique<DiffSubscriptionBuilderT>(
                *context.session, UpdateSpeedT, trading_pair);
        context.diff_subscription_builder->Subscribe();

        auto& notifier = *context.notifier;
        if constexpr (NotifierFlagsT.best_bid_enabled) {
            notifier.AddSubscriber(*(*context.best_bid_notifier));
        }
        if constexpr (NotifierFlagsT.best_ask_enabled) {
            notifier.AddSubscriber(*(*context.best_ask_notifier));
        }
        if constexpr (NotifierFlagsT.best_bid_price_enabled) {
            notifier.AddSubscriber(*(*context.best_bid_price_notifier));
        }
        if constexpr (NotifierFlagsT.best_ask_price_enabled) {
            notifier.AddSubscriber(*(*context.best_ask_price_notifier));
        }

        contexts_.emplace(trading_pair, context_ptr);
    }

    bool SetCallbackOnBestBidChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestBid<Price, Qty>&)> cb) override {
        auto it = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_bid_notifier)
            return false;
        (*(it->second->best_bid_notifier))->SetCallback(cb);
        return true;
    }

    bool SetCallbackOnBestAskChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestAsk<Price, Qty>&)> cb) override {
        auto it = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_ask_notifier)
            return false;
        (*(it->second->best_ask_notifier))->SetCallback(cb);
        return true;
    }

    bool SetCallbackOnBestBidPriceChange(
        aos::TradingPair trading_pair,
        std::function<void(Price&)> cb) override {
        auto it = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_bid_price_notifier)
            return false;
        (*(it->second->best_bid_price_notifier))->SetCallback(cb);
        return true;
    }

    bool SetCallbackOnBestAskPriceChange(
        aos::TradingPair trading_pair,
        std::function<void(Price&)> cb) override {
        auto it = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_ask_price_notifier)
            return false;
        (*(it->second->best_ask_price_notifier))->SetCallback(cb);
        return true;
    }

    void StartAsync() override {
        logi("[BINANCE INFRASTRUCTURE] async start");
        for (auto& [pair, context] : contexts_) {
            if (context) {
                context->session->StartAsync();
            }
        }
    }

    void StopAsync() override { logi("[BINANCE INFRASTRUCTURE] async stop"); }

  private:
    boost::asio::thread_pool& pool_;
    std::unordered_map<aos::TradingPair, std::shared_ptr<ContextT>> contexts_;
};

namespace main_net {
namespace spot {
template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool, auto NotifierFlagsT>
using Infrastructure = InfrastructureImpl<
    Price, Qty, MemoryPoolThreadSafety, MemoryPool,
    aoe::binance::impl::main_net::spot::OrderBookSync<
        Price, Qty, MemoryPoolThreadSafety,
        std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>,
    aoe::binance::impl::diff_response::spot::Listener<Price, Qty,
                                                      MemoryPoolThreadSafety>,
    aoe::binance::impl::main_net::spot::order_book_channel::SessionRW,
    aoe::binance::impl::spot::DiffSubscriptionBuilder,
    aoe::binance::spot::DiffUpdateSpeed_ms,
    aoe::binance::spot::DiffUpdateSpeed_ms::k100, NotifierFlagsT>;
};  // namespace spot
};  // namespace main_net

namespace main_net {
namespace futures {

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool, auto NotifierFlagsT>
using Infrastructure = InfrastructureImpl<
    Price, Qty, MemoryPoolThreadSafety, MemoryPool,
    aoe::binance::impl::main_net::futures::OrderBookSync<
        Price, Qty, MemoryPoolThreadSafety,
        std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>,
    aoe::binance::impl::diff_response::futures::Listener<
        Price, Qty, MemoryPoolThreadSafety>,
    aoe::binance::impl::main_net::futures::order_book_channel::SessionRW,
    aoe::binance::impl::futures::DiffSubscriptionBuilder,
    aoe::binance::futures::DiffUpdateSpeed_ms,
    aoe::binance::futures::DiffUpdateSpeed_ms::k100, NotifierFlagsT>;
};  // namespace futures
};  // namespace main_net
};  // namespace impl
};  // namespace binance
};  // namespace aoe