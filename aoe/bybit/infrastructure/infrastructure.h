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
#include "aos/order_book/order_book.h"
#include "aos/order_book_level/order_book_level.h"
#include "aos/order_book_subscriber/order_book_subscribers.h"
#include "aos/trading_pair/trading_pair.h"
#include "concurrentqueue.h"

namespace aoe {
namespace bybit {
namespace impl {
namespace main_net {
namespace spot {

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool>
struct OrderBookInfraContextDefault {
    moodycamel::ConcurrentQueue<std::vector<char>> queue;
    boost::asio::io_context ioc;
    // Указатели потому что некоторые классы не копируются/не перемещаются
    std::unique_ptr<aos::OrderBookEventListener<
        Price, Qty, MemoryPoolThreadSafety,
        std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>>
        order_book;

    std::unique_ptr<aos::OrderBookNotifier<Price, Qty, MemoryPoolThreadSafety>>
        notifier;

    std::unique_ptr<aoe::bybit::impl::OrderBookSync<
        Price, Qty, MemoryPoolThreadSafety,
        std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>>
        sync;

    std::unique_ptr<aos::BestBidNotifierInterface<Price, Qty>>
        best_bid_notifier;

    std::unique_ptr<aos::BestAskNotifierInterface<Price, Qty>>
        best_ask_notifier;

    std::unique_ptr<aoe::bybit::impl::order_book_response::Listener<
        Price, Qty, MemoryPoolThreadSafety>>
        listener;
    std::unique_ptr<
        aoe::bybit::impl::main_net::spot::order_book_channel::SessionRW>
        session;
    std::unique_ptr<aoe::SubscriptionBuilderInterface>
        diff_subscription_builder;

    std::unique_ptr<aoe::bybit::SingleOrderAPIInterface<MemoryPoolThreadSafety>>
        bybit_api_;

    std::jthread thread;  // must last
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        work_guard_{boost::asio::make_work_guard(
            ioc)};  // unlock ioc. it places in the last
};

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool>
class Infrastructure
    : public aoe::bybit::spot::main_net::InfrastructureInterface,
      public aoe::bybit::spot::main_net::InfrastructureNotifierInterface<Price,
                                                                         Qty>,
      public PlaceOrderInterface<MemoryPoolThreadSafety>,
      public CancelOrderInterface<MemoryPoolThreadSafety> {
    static constexpr bool kFoundInfrastructure_    = true;
    static constexpr bool kNotFoundInfrastructure_ = false;

    boost::asio::io_context ioc_trade_;  // may be need ioc_trade = ioc
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        work_guard_{boost::asio::make_work_guard(ioc_trade_)};
    std::jthread ioc_trade_thread_;
    aoe::bybit::impl::main_net::trade_channel::SessionW trade_ws_session_{
        ioc_trade_};

    aoe::impl::WebSocketSessionProvider<MemoryPoolThreadSafety>
        trade_ws_session_provider_{trade_ws_session_};
    std::unique_ptr<aoe::bybit::SingleOrderAPIInterface<MemoryPoolThreadSafety>>
        bybit_api_{
            std::make_unique<aoe::bybit::impl::external::ws::
                                 SingleOrderAPIDefault<MemoryPoolThreadSafety>>(
                trade_ws_session_provider_)};
    boost::asio::thread_pool& pool_;
    std::unordered_map<aos::TradingPair,
                       std::shared_ptr<OrderBookInfraContextDefault<
                           Price, Qty, MemoryPoolThreadSafety, MemoryPool>>>
        contexts_;

  public:
    explicit Infrastructure(boost::asio::thread_pool& pool)
        : pool_(pool), ioc_trade_thread_([this]() { ioc_trade_.run(); }) {}
    ~Infrastructure() override { work_guard_.reset(); };
    void Register(aos::TradingPair trading_pair) override {
        if (contexts_.contains(trading_pair)) return;

        auto context_ptr   = std::make_shared<OrderBookInfraContextDefault<
              Price, Qty, MemoryPoolThreadSafety, MemoryPool>>();
        auto& context      = *context_ptr;
        auto& queue        = context.queue;

        context.order_book = std::make_unique<aos::OrderBookEventListener<
            Price, Qty, MemoryPoolThreadSafety,
            std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>>(pool_,
                                                                          1000);

        context.notifier   = std::make_unique<
              aos::OrderBookNotifier<Price, Qty, MemoryPoolThreadSafety>>(
            *context.order_book);

        context.sync = std::make_unique<aoe::bybit::impl::OrderBookSync<
            Price, Qty, MemoryPoolThreadSafety,
            std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>>(
            pool_, *context.notifier);

        context.best_bid_notifier =
            std::make_unique<aos::impl::BestBidNotifier<Price, Qty>>(
                pool_, *context.order_book);

        context.best_ask_notifier =
            std::make_unique<aos::impl::BestAskNotifier<Price, Qty>>(
                pool_, *context.order_book);

        context.listener =
            std::make_unique<aoe::bybit::impl::order_book_response::Listener<
                Price, Qty, MemoryPoolThreadSafety>>(pool_, queue,
                                                     *context.sync);
        // run ioc
        context.thread =
            std::jthread([this, ioc_ptr = &context.ioc]() { ioc_ptr->run(); });

        context.session = std::make_unique<
            aoe::bybit::impl::main_net::spot::order_book_channel::SessionRW>(
            context.ioc, queue, *context.listener);

        context.diff_subscription_builder = std::make_unique<
            aoe::bybit::impl::spot::OrderBookSubscriptionBuilder>(
            *context.session, aoe::bybit::spot::Depth::k200, trading_pair);

        context.diff_subscription_builder->Subscribe();

        // Add subscribers
        auto& notifier = *context.notifier;
        // notifier.AddSubscriber(
        //     aos::impl::BestBidOrBestAskNotifier<double, double>{
        //         pool_, *context.order_book});
        notifier.AddSubscriber(*context.best_bid_notifier);
        notifier.AddSubscriber(*context.best_ask_notifier);

        // std::this_thread::sleep_for(
        //     std::chrono::seconds(100));  // спит 1 секунду
        contexts_.emplace(trading_pair, context_ptr);
    }
    bool SetCallbackOnBestBidChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestBid<Price, Qty>& new_bid)> cb) override {
        auto it = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_bid_notifier) {
            return kNotFoundInfrastructure_;
        }
        it->second->best_bid_notifier->SetCallback(cb);
        return kFoundInfrastructure_;
    }

    bool SetCallbackOnBestAskChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestAsk<Price, Qty>& new_ask)> cb) override {
        constexpr bool kFoundInfrastructure    = true;
        constexpr bool kNotFoundInfrastructure = false;

        auto it                                = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_ask_notifier) {
            return kNotFoundInfrastructure_;
        }
        it->second->best_ask_notifier->SetCallback(cb);
        return kFoundInfrastructure_;
    }
    void PlaceOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPoolThreadSafety>>
            request) override {
        // bybit_api_->PlaceOrder(request);
    };
    void CancelOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPoolThreadSafety>>
            request) override {
        // bybit_api_->CancelOrder(request);
    };
};
};  // namespace spot
};  // namespace main_net

namespace main_net {
namespace linear {

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool>
struct OrderBookInfraContextDefault {
    moodycamel::ConcurrentQueue<std::vector<char>> queue;
    boost::asio::io_context ioc;

    // Указатели потому что некоторые классы не копируются/не перемещаются
    std::unique_ptr<aos::OrderBookEventListener<
        Price, Qty, MemoryPoolThreadSafety,
        std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>>
        order_book;

    std::unique_ptr<aos::OrderBookNotifier<Price, Qty, MemoryPoolThreadSafety>>
        notifier;

    std::unique_ptr<aoe::bybit::impl::OrderBookSync<
        Price, Qty, MemoryPoolThreadSafety,
        std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>>
        sync;

    std::unique_ptr<aos::BestBidNotifierInterface<Price, Qty>>
        best_bid_notifier;

    std::unique_ptr<aos::BestAskNotifierInterface<Price, Qty>>
        best_ask_notifier;

    std::unique_ptr<aoe::bybit::impl::order_book_response::Listener<
        Price, Qty, MemoryPoolThreadSafety>>
        listener;
    std::unique_ptr<
        aoe::bybit::impl::main_net::linear::order_book_channel::SessionRW>
        session;
    std::unique_ptr<aoe::SubscriptionBuilderInterface>
        diff_subscription_builder;

    std::jthread thread;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        work_guard_{boost::asio::make_work_guard(
            ioc)};  // unlock ioc. it places in the last
};

template <typename Price, typename Qty,
          template <typename> typename MemoryPoolThreadSafety,
          template <typename> typename MemoryPool>
class Infrastructure
    : public aoe::bybit::linear::main_net::InfrastructureInterface,
      public aoe::bybit::linear::main_net::InfrastructureNotifierInterface<
          Price, Qty>,
      public PlaceOrderInterface<MemoryPoolThreadSafety>,
      public CancelOrderInterface<MemoryPoolThreadSafety> {
    static constexpr bool kFoundInfrastructure_    = true;
    static constexpr bool kNotFoundInfrastructure_ = false;

    boost::asio::io_context ioc_trade_;  // may be need ioc_trade = ioc
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        work_guard_{boost::asio::make_work_guard(ioc_trade_)};
    std::jthread ioc_trade_thread_;
    aoe::bybit::impl::main_net::trade_channel::SessionW trade_ws_session_{
        ioc_trade_};
    aoe::impl::WebSocketSessionProvider<MemoryPoolThreadSafety>
        trade_ws_session_provider_{trade_ws_session_};
    std::unique_ptr<aoe::bybit::SingleOrderAPIInterface<MemoryPoolThreadSafety>>
        bybit_api_{
            std::make_unique<aoe::bybit::impl::external::ws::
                                 SingleOrderAPIDefault<MemoryPoolThreadSafety>>(
                trade_ws_session_provider_)};

  public:
    explicit Infrastructure(boost::asio::thread_pool& pool)
        : pool_(pool), ioc_trade_thread_([this]() { ioc_trade_.run(); }) {}
    ~Infrastructure() override {
        work_guard_.reset();  // unlock infinite spin ioc_trade_thread_
    };
    void Register(aos::TradingPair trading_pair) override {
        if (contexts_.contains(trading_pair)) return;

        auto context_ptr   = std::make_shared<OrderBookInfraContextDefault<
              Price, Qty, MemoryPoolThreadSafety, MemoryPool>>();
        auto& context      = *context_ptr;
        auto& queue        = context.queue;

        context.order_book = std::make_unique<aos::OrderBookEventListener<
            Price, Qty, MemoryPoolThreadSafety,
            std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>>(pool_,
                                                                          1000);

        context.notifier   = std::make_unique<
              aos::OrderBookNotifier<Price, Qty, MemoryPoolThreadSafety>>(
            *context.order_book);

        context.sync = std::make_unique<aoe::bybit::impl::OrderBookSync<
            Price, Qty, MemoryPoolThreadSafety,
            std::unordered_map<Price, aos::OrderBookLevel<Price, Qty>*>>>(
            pool_, *context.notifier);

        context.best_bid_notifier =
            std::make_unique<aos::impl::BestBidNotifier<Price, Qty>>(
                pool_, *context.order_book);

        context.best_ask_notifier =
            std::make_unique<aos::impl::BestAskNotifier<Price, Qty>>(
                pool_, *context.order_book);

        context.listener =
            std::make_unique<aoe::bybit::impl::order_book_response::Listener<
                Price, Qty, MemoryPoolThreadSafety>>(pool_, queue,
                                                     *context.sync);
        // run ioc before init session
        context.thread =
            std::jthread([ioc_ptr = &context.ioc]() { ioc_ptr->run(); });

        context.session = std::make_unique<
            aoe::bybit::impl::main_net::linear::order_book_channel::SessionRW>(
            context.ioc, queue, *context.listener);
        /// @brief Creates a subscription builder for linear order book diff
        /// updates.
        ///
        /// A fixed update speed of `k100` (100 ms) is used here, which
        /// determines how frequently the exchange sends order book updates.
        ///
        /// @note The `k100` value may not be optimal for all trading pairs.
        /// Some pairs may have lower liquidity or activity, and in such cases,
        /// a slower update speed (e.g., `k250` or `k500`) might be more
        /// appropriate. In the future, it may be worth implementing logic to
        /// determine the optimal update speed based on trading pair
        /// characteristics.
        ///
        /// @param session The WebSocket session used to send the subscription.
        /// @param trading_pair The trading pair to subscribe to.
        context.diff_subscription_builder = std::make_unique<
            aoe::bybit::impl::linear::OrderBookSubscriptionBuilder>(
            *context.session, aoe::bybit::linear::Depth::k500, trading_pair);
        context.diff_subscription_builder->Subscribe();

        // Add subscribers
        auto& notifier = *context.notifier;
        // notifier.AddSubscriber(
        //     aos::impl::BestBidOrBestAskNotifier<double, double>{
        //         pool_, *context.order_book});
        notifier.AddSubscriber(*context.best_bid_notifier);
        notifier.AddSubscriber(*context.best_ask_notifier);

        // std::this_thread::sleep_for(
        //     std::chrono::seconds(100));  // спит 1 секунду
        contexts_.emplace(trading_pair, context_ptr);
    }
    bool SetCallbackOnBestBidChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestBid<Price, Qty>& new_bid)> cb) override {
        auto it = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_bid_notifier) {
            return kNotFoundInfrastructure_;
        }
        it->second->best_bid_notifier->SetCallback(cb);
        return kFoundInfrastructure_;
    }

    bool SetCallbackOnBestAskChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestAsk<Price, Qty>& new_ask)> cb) override {
        auto it = contexts_.find(trading_pair);
        if (it == contexts_.end() || !it->second ||
            !it->second->best_ask_notifier) {
            return kNotFoundInfrastructure_;
        }
        it->second->best_ask_notifier->SetCallback(cb);
        return kFoundInfrastructure_;
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

  private:
    boost::asio::thread_pool& pool_;
    std::unordered_map<aos::TradingPair,
                       std::shared_ptr<OrderBookInfraContextDefault<
                           Price, Qty, MemoryPoolThreadSafety, MemoryPool>>>
        contexts_;
};
};  // namespace linear
};  // namespace main_net
};  // namespace impl
};  // namespace bybit
};  // namespace aoe