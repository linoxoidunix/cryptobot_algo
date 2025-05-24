#pragma once
#include "aoe/binance/constants/constants.h"
#include "aoe/binance/order_book_sync/i_order_book_sync.h"
#include "aoe/binance/parser/json/ws/diff_response/i_parser.h"
#include "aoe/response_queue_listener/i_response_queue_listener.h"
#include "aot/Logger.h"
#include "boost/asio.hpp"
#include "concurrentqueue.h"
#include "simdjson.h"

namespace aoe {
namespace binance {
namespace impl {
namespace diff_response {
namespace spot {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class Listener : public ResponseQueueListenerInterface {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    moodycamel::ConcurrentQueue<std::vector<char>>& queue_;
    aoe::binance::spot::impl::DiffEventParser<Price, Qty, MemoryPool> parser_{
        kMaximumDiffEventsFromExchange};
    aoe::binance::spot::OrderBookSyncInterface<Price, Qty, MemoryPool>& sync_;

  public:
    Listener(boost::asio::thread_pool& thread_pool,
             moodycamel::ConcurrentQueue<std::vector<char>>& queue,
             aoe::binance::spot::OrderBookSyncInterface<Price, Qty, MemoryPool>&
                 sync)
        : strand_(boost::asio::make_strand(thread_pool)),
          queue_(queue),
          sync_(sync) {}
    void OnDataEnqueued() override {
        boost::asio::co_spawn(strand_, ProcessQueue(), boost::asio::detached);
    }
    ~Listener() override = default;

  private:
    boost::asio::awaitable<void> ProcessQueue() {
        std::vector<char> msg;
        simdjson::ondemand::parser parser;
        while (queue_.try_dequeue(msg)) {
            logi("process message of size {}", msg.size());
            simdjson::padded_string padded_json(msg.data(), msg.size());
            simdjson::ondemand::document doc;
            simdjson::error_code error = parser.iterate(padded_json).get(doc);
            std::string str(msg.data(), msg.size());
            logi("parsed JSON: {}", str);
            if (error) {
                logi("parsing error: {}", simdjson::error_message(error));
                co_return;
            }
            auto [status, ptr] = parser_.ParseAndCreate(doc);
            logd("status_parsed:{}", status);
            if (!status) {
                co_return;
            }
            sync_.AcceptDiff(ptr);
        }
        co_return;
    }
};
};  // namespace spot
namespace futures {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class Listener : public ResponseQueueListenerInterface {
    aoe::binance::futures::impl::DiffEventParser<Price, Qty, MemoryPool>
        parser_{kMaximumDiffEventsFromExchange};
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    moodycamel::ConcurrentQueue<std::vector<char>>& queue_;

    aoe::binance::futures::OrderBookSyncInterface<Price, Qty, MemoryPool>&
        sync_;

  public:
    Listener(boost::asio::thread_pool& thread_pool,
             moodycamel::ConcurrentQueue<std::vector<char>>& queue,
             aoe::binance::futures::OrderBookSyncInterface<Price, Qty,
                                                           MemoryPool>& sync)
        : strand_(boost::asio::make_strand(thread_pool)),
          queue_(queue),
          sync_(sync) {}
    void OnDataEnqueued() override {
        boost::asio::co_spawn(strand_, ProcessQueue(), boost::asio::detached);
    }
    ~Listener() override = default;

  private:
    boost::asio::awaitable<void> ProcessQueue() {
        std::vector<char> msg;
        simdjson::ondemand::parser parser;
        while (queue_.try_dequeue(msg)) {
            logi("process message of size {}", msg.size());
            simdjson::padded_string padded_json(msg.data(), msg.size());
            simdjson::ondemand::document doc;
            simdjson::error_code error = parser.iterate(padded_json).get(doc);
            std::string str(msg.data(), msg.size());
            logi("parsed JSON: {}", str);
            if (error) {
                logi("parsing error: {}", simdjson::error_message(error));
                co_return;
            }
            auto [status, ptr] = parser_.ParseAndCreate(doc);
            logd("status_parsed:{}", status);
            if (!status) {
                co_return;
            }
            sync_.AcceptDiff(ptr);
        }
        co_return;
    }
};
};  // namespace futures
};  // namespace diff_response
};  // namespace impl
};  // namespace binance
};  // namespace aoe