#pragma once
// #include <memory>
// #include <thread>
// #include <unordered_map>

// // #include "aoe/binance/enums/enums.h"
// // #include "aoe/bybit/infrastructure/i_infrastructure.h"
// // #include "aos/common/network_environment.h"
// // #include "aos/trading_pair/trading_pair.h"
// #include "aoe/aoe.h"
// #include "aos/aos.h"
// #include "concurrentqueue.h"

// namespace aoe {
// namespace binance {
// namespace impl {
// namespace main_net {
// namespace spot {

// template <typename Price, typename Qty,
//           template <typename> typename MemoryPoolThreadSafety,
//           template <typename> typename MemoryPool>
// struct OrderBookInfraContextDefault {
//     moodycamel::ConcurrentQueue<std::vector<char>> queue;
//     boost::asio::io_context ioc;
//     std::jthread thread;

//     // Указатели потому что некоторые классы не копируются/не перемещаются
//     std::unique_ptr<aos::OrderBookEventListener<
//         double, double, common::MemoryPoolThreadSafety,
//         std::unordered_map<double, aos::OrderBookLevel<double, double>*>>>
//         order_book;

//     std::unique_ptr<
//         aos::OrderBookNotifier<double, double,
//         common::MemoryPoolThreadSafety>> notifier;

//     std::unique_ptr<aoe::binance::impl::main_net::spot::OrderBookSync<
//         double, double, common::MemoryPoolThreadSafety,
//         std::unordered_map<double, aos::OrderBookLevel<double, double>*>>>
//         sync;

//     std::unique_ptr<aoe::binance::impl::diff_response::spot::Listener<
//         double, double, common::MemoryPoolThreadSafety>>
//         listener;

//     std::unique_ptr<
//         aoe::binance::impl::main_net::spot::order_book_channel::SessionRW>
//         session;
// };

// class Infrastructure : public InfrastructureInterface {
//   public:
//     explicit Infrastructure(boost::asio::thread_pool& pool) : pool_(pool) {}

//     void Register(aos::TradingPair trading_pair) override {
//         if (contexts_.contains(trading_pair)) return;

//         OrderBookInfraContext context;
//         auto& queue        = context.queue;

//         context.order_book = std::make_unique<aos::OrderBookEventListener<
//             double, double, common::MemoryPoolThreadSafety,
//             std::unordered_map<double, aos::OrderBookLevel<double,
//             double>*>>>( pool_, 1000);

//         context.notifier = std::make_unique<aos::OrderBookNotifier<
//             double, double, common::MemoryPoolThreadSafety>>(
//             *context.order_book);

//         context.sync =
//             std::make_unique<aoe::binance::impl::main_net::spot::OrderBookSync<
//                 double, double, common::MemoryPoolThreadSafety,
//                 std::unordered_map<double,
//                                    aos::OrderBookLevel<double, double>*>>>(
//                 pool_, *context.notifier);

//         context.listener =
//             std::make_unique<aoe::binance::impl::diff_response::spot::Listener<
//                 double, double, common::MemoryPoolThreadSafety>>(pool_,
//                 queue,
//                                                                  *context.sync);

//         context.session = std::make_unique<
//             aoe::binance::impl::main_net::spot::order_book_channel::SessionRW>(
//             context.ioc, queue, *context.listener);

//         aoe::binance::impl::spot::DiffSubscriptionBuilder builder{
//             *context.session, aoe::binance::spot::DiffUpdateSpeed_ms::k100,
//             trading_pair};
//         builder.Subscribe();

//         // Add subscribers
//         auto& notifier = *context.notifier;
//         // notifier.AddSubscriber(
//         //     aos::impl::BestBidOrBestAskNotifier<double, double>{
//         //         pool_, *context.order_book});
//         notifier.AddSubscriber(aos::impl::BestBidNotifier<double, double>{
//             pool_, *context.order_book});
//         notifier.AddSubscriber(aos::impl::BestAskNotifier<double, double>{
//             pool_, *context.order_book});

//         // run ioc
//         context.thread =
//             std::jthread([ioc_ptr = &context.ioc]() { ioc_ptr->run(); });

//         contexts_.emplace(trading_pair, std::move(context));
//     }

//   private:
//     boost::asio::thread_pool& pool_;
//     std::unordered_map<aos::TradingPair, OrderBookInfraContext> contexts_;
// };
// };  // namespace spot
// };  // namespace main_net
// };  // namespace impl
// };  // namespace binance
// };  // namespace aoe