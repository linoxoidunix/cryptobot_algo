// #pragma once
// #include "aoe/bybit/constants/constants.h"
// #include "aoe/bybit/events_acceptor/funding_rate_event/i_acceptor.h"
// #include "aoe/bybit/parser/json/ws/ticker_response/funding_rate/parser.h"
// #include "aoe/response_queue_listener/i_response_queue_listener.h"
// #include "aos/logger/mylog.h"
// #include "boost/asio.hpp"
// #include "concurrentqueue.h"
// #include "simdjson.h"  // NOLINT

// namespace aoe {
// namespace bybit {
// namespace impl {
// namespace ticker_response {
// namespace funding_rate {

// template <template <typename> typename MemoryPoolThreadSafety>
// class Listener : public ResponseQueueListenerInterface {
//     boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
//     moodycamel::ConcurrentQueue<std::vector<char>>& queue_;
//     FundingRateEventParser<MemoryPoolThreadSafety> parser_{
//         kMaximumLinearTickers};
//     FundingRateEventAcceptorInterface<MemoryPoolThreadSafety> acceptor_;

//   public:
//     Listener(
//         boost::asio::thread_pool& thread_pool,
//         moodycamel::ConcurrentQueue<std::vector<char>>& queue,
//         FundingRateEventAcceptorInterface<MemoryPoolThreadSafety>& acceptor)
//         : strand_(boost::asio::make_strand(thread_pool)),
//           queue_(queue),
//           acceptor_(acceptor) {}
//     void OnDataEnqueued() override {
//         boost::asio::co_spawn(strand_, ProcessQueue(),
//         boost::asio::detached);
//     }
//     ~Listener() override = default;

//   private:
//     boost::asio::awaitable<void> ProcessQueue() {
//         std::vector<char> msg;
//         simdjson::ondemand::parser parser;
//         while (queue_.try_dequeue(msg)) {
//             logi("process message of size {}", msg.size());
//             simdjson::padded_string padded_json(msg.data(), msg.size());
//             simdjson::ondemand::document doc;
//             simdjson::error_code error =
//             parser.iterate(padded_json).get(doc);
//             // std::string str(msg.data(), msg.size());
//             // logi("parsed JSON: {}", str);
//             if (error) {
//                 logi("parsing error: {}", simdjson::error_message(error));
//                 co_return;
//             }
//             auto [status, ptr] = parser_.ParseAndCreate(doc);
//             if (!status) co_return;
//             acceptor_.OnEvent(ptr);
//         }
//         co_return;
//     }
// };
// };  // namespace funding_rate
// };  // namespace ticker_response
// };  // namespace impl
// };  // namespace bybit
// };  // namespace aoe