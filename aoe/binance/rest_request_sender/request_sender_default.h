// #pragma once
// #include <thread>
// #include <vector>

// #include "aoe/binance/order_book_sync/i_order_book_sync.h"
// #include "aoe/binance/parser/json/rest/snapshot_response/i_parser.h"
// #include "aoe/binance/request/get_snapshot/request.h"
// #include "concurrentqueue.h"

// namespace aoe {
// namespace binance {
// namespace main_net {
// namespace spot {
// namespace impl {
// template <template <typename> typename MemoryPool>
// class SnapshotRequester {
//     boost::asio::io_context ioc_;
//     moodycamel::ConcurrentQueue<std::vector<char>> queue_;
//     std::jthread thread_;
//     aoe::binance::impl::SnapshotEventParser<double, double,
//                                             common::MemoryPoolThreadSafety>
//         parser_;
//     aoe::binance::impl::snapshot::Listener<double, double,
//                                            common::MemoryPoolThreadSafety>
//         listener_;
//     aoe::binance::impl::main_net::spot::RestSessionRW session_;
//     aoe::binance::snapshot::impl::SnapshotRequestSender<MemoryPool> sender_;
//     aoe::binance::OrderBookSyncInterface<Price, Qty, MemoryPool>& sync;

//   public:
//     SnapshotRequester(
//         boost::asio::thread_pool& thread_pool,
//         aos::OrderBookEventListener<
//             double, double, common::MemoryPoolThreadSafety,
//             std::unordered_map<double, aos::OrderBookLevel<double,
//             double*>>>& order_book);

//     void Send(aos::TradingPair pair) {}

//   private:
//     boost::asio::io_context ioc_;
//     moodycamel::ConcurrentQueue<std::vector<char>> queue_;
//     std::jthread thread_;
//     aoe::binance::impl::SnapshotEventParser<double, double,
//                                             common::MemoryPoolThreadSafety>
//         parser_;
//     aoe::binance::impl::snapshot::Listener<double, double,
//                                            common::MemoryPoolThreadSafety>
//         listener_;
//     aoe::binance::impl::main_net::spot::RestSessionRW session_;
//     aoe::binance::snapshot::impl::SnapshotRequestSender<
//         common::MemoryPoolThreadSafety>
//         sender_;
// };

// };  // namespace impl
// };  // namespace spot
// };  // namespace main_net
// };  // namespace binance
// };  // namespace aoe