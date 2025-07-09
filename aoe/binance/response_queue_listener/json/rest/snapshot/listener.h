#pragma once
#include "aoe/binance/order_book_sync/i_order_book_sync.h"
#include "aoe/binance/parser/json/rest/snapshot_response/i_parser.h"
#include "aoe/response_queue_listener/i_response_queue_listener.h"
#include "aos/logger/mylog.h"
#include "boost/asio.hpp"
#include "concurrentqueue.h"
#include "simdjson.h"  // NOLINT

namespace aoe {
namespace binance {
namespace impl {
namespace snapshot {
namespace spot {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class Listener : public ResponseQueueListenerInterface {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    moodycamel::ConcurrentQueue<std::vector<char>>& queue_;
    SnapshotEventParserInterface<Price, Qty, MemoryPool>& parser_;
    aoe::binance::spot::OrderBookSyncInterface<Price, Qty, MemoryPool>& sync_;
    aos::TradingPair trading_pair_ = aos::TradingPair::kCount;

  public:
    Listener(boost::asio::thread_pool& thread_pool,
             moodycamel::ConcurrentQueue<std::vector<char>>& queue,
             SnapshotEventParserInterface<Price, Qty, MemoryPool>& parser,
             aoe::binance::spot::OrderBookSyncInterface<Price, Qty, MemoryPool>&
                 sync)
        : strand_(boost::asio::make_strand(thread_pool)),
          queue_(queue),
          parser_(parser),
          sync_(sync) {}
    void OnDataEnqueued() override {
        boost::asio::co_spawn(strand_, ProcessQueue(), boost::asio::detached);
    }
    ~Listener() override = default;
    void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    }

  private:
    boost::asio::awaitable<void> ProcessQueue() {
        std::vector<char> msg;
        simdjson::ondemand::parser parser;
        while (queue_.try_dequeue(msg)) {
            logi("process message of size {}", msg.size());
            simdjson::padded_string padded_json(msg.data(), msg.size());
            simdjson::ondemand::document doc;
            simdjson::error_code error = parser.iterate(padded_json).get(doc);
            // std::string str(msg.data(), msg.size());
            //  logi("parsed snapshot JSON: {}", str);
            if (error) {
                logi("parsing error: {}", simdjson::error_message(error));
                co_return;
            }
            auto [status, ptr] = parser_.ParseAndCreate(doc);
            logd("status_parsed_snapshot:{}", status);
            if (!status) co_return;
            logd("send snapshot to sync");
            ptr->SetTradingPair(trading_pair_);
            sync_.AcceptSnapshot(ptr);
        }
        co_return;
    }
};
};  // namespace spot
namespace futures {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class Listener : public ResponseQueueListenerInterface {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    moodycamel::ConcurrentQueue<std::vector<char>>& queue_;
    SnapshotEventParserInterface<Price, Qty, MemoryPool>& parser_;
    aoe::binance::futures::OrderBookSyncInterface<Price, Qty, MemoryPool>&
        sync_;
    aos::TradingPair trading_pair_ = aos::TradingPair::kCount;

  public:
    Listener(boost::asio::thread_pool& thread_pool,
             moodycamel::ConcurrentQueue<std::vector<char>>& queue,
             SnapshotEventParserInterface<Price, Qty, MemoryPool>& parser,
             aoe::binance::futures::OrderBookSyncInterface<Price, Qty,
                                                           MemoryPool>& sync)
        : strand_(boost::asio::make_strand(thread_pool)),
          queue_(queue),
          parser_(parser),
          sync_(sync) {}
    void OnDataEnqueued() override {
        boost::asio::co_spawn(strand_, ProcessQueue(), boost::asio::detached);
    }
    void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
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
            logi("parsed snapshot as JSON: {}", str);
            if (error) {
                logi("parsing error: {}", simdjson::error_message(error));
                co_return;
            }
            auto [status, ptr] = parser_.ParseAndCreate(doc);
            logd("status_parsed_snapshot:{}", status);
            if (!status) co_return;
            logd("send snapshot to sync");
            ptr->SetTradingPair(trading_pair_);
            sync_.AcceptSnapshot(ptr);
        }
        co_return;
    }
};
};  // namespace futures
};  // namespace snapshot
};  // namespace impl
};  // namespace binance
};  // namespace aoe