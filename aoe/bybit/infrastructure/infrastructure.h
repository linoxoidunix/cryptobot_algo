#pragma once
#include <memory>
#include <thread>
#include <unordered_map>

#include "aoe/api/i_exchange_api.h"
#include "aoe/bybit/api/external/web_socket/c_exchange_api.h"
#include "aoe/bybit/infrastructure/i_infrastructure.h"
#include "aoe/bybit/order_book_sync/order_book_sync.h"
#include "aoe/bybit/response_queue_listener/json/ws/order_book_response/listener.h"
#include "aoe/bybit/session/web_socket/web_socket.h"
#include "aoe/bybit/subscription_builder/subscription_builder.h"
#include "aoe/session_provider/web_socket/web_socket_session_provider.h"
#include "aoe/subscription_builder/i_subscription_builder.h"
#include "aos/order_book/order_book.h"
#include "aos/order_book_level/order_book_level.h"
#include "aos/order_book_subscriber/order_book_subscribers.h"
#include "aos/trading_pair/trading_pair.h"
#include "concurrentqueue.h"

namespace aoe {
namespace bybit {
namespace impl {

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool, typename OrderBookSyncT,
          typename ListenerT, typename OrderBookDiffSessionRWType>
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
    std::unique_ptr<OrderBookDiffSessionRWType> session;
    std::unique_ptr<aoe::SubscriptionBuilderInterface>
        diff_subscription_builder;

    std::jthread thread;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        work_guard_{boost::asio::make_work_guard(ioc)};
};

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool, typename OrderBookSyncT,
          typename ListenerT, typename OrderBookDiffSessionRWType,
          typename OrderBookSubscriptionBuilderT, typename DepthTypeT,
          DepthTypeT DepthT, typename TradeSessionRType, auto NotifierFlagsType>
class InfrastructureImpl : public InfrastructureInterface,
                           public InfrastructureNotifierInterface<Price, Qty>,
                           public PlaceOrderInterface<MemoryPoolThreadSafety>,
                           public CancelOrderInterface<MemoryPoolThreadSafety> {
    using ContextT =
        OrderBookInfraContext<Price, Qty, MemoryPoolThreadSafety, MemoryPool,
                              OrderBookSyncT, ListenerT,
                              OrderBookDiffSessionRWType>;

    static constexpr bool kFoundInfrastructure    = true;
    static constexpr bool kNotFoundInfrastructure = false;

    boost::asio::io_context ioc_trade_;  // may be need ioc_trade = ioc
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        work_guard_{boost::asio::make_work_guard(ioc_trade_)};
    std::jthread ioc_trade_thread_;
    TradeSessionRType trade_ws_session_{ioc_trade_};
    aoe::impl::WebSocketSessionProvider<MemoryPoolThreadSafety>
        trade_ws_session_provider_{trade_ws_session_};
    std::unique_ptr<aoe::bybit::SingleOrderAPIInterface<MemoryPoolThreadSafety>>
        bybit_api_{
            std::make_unique<aoe::bybit::impl::external::ws::
                                 SingleOrderAPIDefault<MemoryPoolThreadSafety>>(
                trade_ws_session_provider_)};

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

        if constexpr (NotifierFlagsType.best_bid_enabled) {
            context.best_bid_notifier =
                std::make_unique<aos::impl::BestBidNotifier<Price, Qty>>(
                    pool_, *context.order_book);
        }

        if constexpr (NotifierFlagsType.best_ask_enabled) {
            context.best_ask_notifier =
                std::make_unique<aos::impl::BestAskNotifier<Price, Qty>>(
                    pool_, *context.order_book);
        }

        if constexpr (NotifierFlagsType.best_bid_price_enabled) {
            context.best_bid_price_notifier =
                std::make_unique<aos::impl::BestBidPriceNotifier<Price, Qty>>(
                    pool_, *context.order_book);
        }

        if constexpr (NotifierFlagsType.best_ask_price_enabled) {
            context.best_ask_price_notifier =
                std::make_unique<aos::impl::BestAskPriceNotifier<Price, Qty>>(
                    pool_, *context.order_book);
        }

        context.listener =
            std::make_unique<ListenerT>(pool_, queue, *context.sync);

        context.thread =
            std::jthread([ioc_ptr = &context.ioc]() { ioc_ptr->run(); });

        context.session = std::make_unique<OrderBookDiffSessionRWType>(
            context.ioc, queue, *context.listener);

        /// @brief Creates a subscription builder for linear order book diff
        /// updates.
        ///
        /// A fixed update speed of `k100` (100 ms) is used here, which
        /// determines how frequently the exchange sends order book updates.
        ///
        /// @note The `k100` value may not be optimal for all trading pairs.
        /// Some pairs may have lower liquidity or activity, and in such cases
        /// a slower update speed (e.g., `k250` or `k500`) might be more
        /// appropriate. In the future, it may be worth implementing logic to
        /// determine the optimal update speed based on trading pair
        /// characteristics.
        ///
        context.diff_subscription_builder =
            std::make_unique<OrderBookSubscriptionBuilderT>(
                *context.session, DepthT, trading_pair);
        context.diff_subscription_builder->Subscribe();

        auto& notifier = *context.notifier;
        if constexpr (NotifierFlagsType.best_bid_enabled) {
            notifier.AddSubscriber(*(*context.best_bid_notifier));
        }
        if constexpr (NotifierFlagsType.best_ask_enabled) {
            notifier.AddSubscriber(*(*context.best_ask_notifier));
        }
        if constexpr (NotifierFlagsType.best_bid_price_enabled) {
            notifier.AddSubscriber(*(*context.best_bid_price_notifier));
        }
        if constexpr (NotifierFlagsType.best_ask_price_enabled) {
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
            return kNotFoundInfrastructure;
        (*(it->second->best_bid_notifier))->SetCallback(cb);
        return kFoundInfrastructure;
    }

    bool SetCallbackOnBestAskChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestAsk<Price, Qty>&)> cb) override {
        auto it = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_ask_notifier)
            return kNotFoundInfrastructure;
        (*(it->second->best_ask_notifier))->SetCallback(cb);
        return kFoundInfrastructure;
    }

    bool SetCallbackOnBestBidPriceChange(
        aos::TradingPair trading_pair,
        std::function<void(Price&)> cb) override {
        auto it = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_bid_price_notifier)
            return kNotFoundInfrastructure;
        (*(it->second->best_bid_price_notifier))->SetCallback(cb);
        return kFoundInfrastructure;
    }

    bool SetCallbackOnBestAskPriceChange(
        aos::TradingPair trading_pair,
        std::function<void(Price&)> cb) override {
        auto it = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_ask_price_notifier)
            return kNotFoundInfrastructure;
        (*(it->second->best_ask_price_notifier))->SetCallback(cb);
        return kFoundInfrastructure;
    }

    void PlaceOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPoolThreadSafety>>
            request) override {
        bybit_api_->PlaceOrder(request);
    };
    void CancelOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPoolThreadSafety>>
            request) override {
        bybit_api_->CancelOrder(request);
    };

    void StartAsync() override {
        logi("[BYBIT INFRASTRUCTURE] async start");
        for (auto& [pair, context] : contexts_) {
            if (context) {
                context->session->StartAsync();
            }
        }
        trade_ws_session_.StartAsync();
    }

    void StopAsync() override { logi("[BYBIT INFRASTRUCTURE] async stop"); }

  private:
    boost::asio::thread_pool& pool_;
    std::unordered_map<aos::TradingPair, std::shared_ptr<ContextT>> contexts_;
};

namespace main_net {
namespace spot {

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool, auto NotifierFlagsType>
using Infrastructure = InfrastructureImpl<
    Price, Qty, MemoryPoolThreadSafety, MemoryPool,
    aoe::bybit::impl::OrderBookSync<
        Price, Qty, MemoryPoolThreadSafety,
        std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>,
    aoe::bybit::impl::order_book_response::Listener<Price, Qty,
                                                    MemoryPoolThreadSafety>,
    aoe::bybit::impl::main_net::spot::order_book_channel::SessionRW,
    aoe::bybit::impl::spot::OrderBookSubscriptionBuilder,
    aoe::bybit::spot::Depth, aoe::bybit::spot::Depth::k200,
    aoe::bybit::impl::main_net::trade_channel::SessionW, NotifierFlagsType>;
};  // namespace spot
};  // namespace main_net

namespace main_net {
namespace linear {

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool, auto NotifierFlagsType>
using Infrastructure = InfrastructureImpl<
    Price, Qty, MemoryPoolThreadSafety, MemoryPool,
    aoe::bybit::impl::OrderBookSync<
        Price, Qty, MemoryPoolThreadSafety,
        std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>,
    aoe::bybit::impl::order_book_response::Listener<Price, Qty,
                                                    MemoryPoolThreadSafety>,
    aoe::bybit::impl::main_net::linear::order_book_channel::SessionRW,
    aoe::bybit::impl::linear::OrderBookSubscriptionBuilder,
    aoe::bybit::linear::Depth, aoe::bybit::linear::Depth::k500,
    aoe::bybit::impl::main_net::trade_channel::SessionW, NotifierFlagsType>;
};  // namespace linear
};  // namespace main_net
};  // namespace impl
};  // namespace bybit
};  // namespace aoe
