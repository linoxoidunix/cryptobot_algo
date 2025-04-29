#pragma once
#include "aoe/bybit/order_watcher/i_order_watcher.h"
#include "aoe/bybit/parser/json/ws/order_response/i_order_event_parser.h"
#include "aoe/response_queue_listener/i_response_queue_listener.h"
#include "aot/Logger.h"
#include "boost/asio.hpp"
#include "concurrentqueue.h"
#include "simdjson.h"

namespace aoe {
namespace bybit {
namespace impl {
namespace order_response {

template <template <typename> typename MemoryPool>
class Listener : public ResponseQueueListenerInterface {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    moodycamel::ConcurrentQueue<std::vector<char>>& queue_;
    OrderEventParserInterface<MemoryPool>& parser_;
    OrderWatcherInterface<MemoryPool>& watcher_;

  public:
    Listener(boost::asio::thread_pool& thread_pool,
             moodycamel::ConcurrentQueue<std::vector<char>>& queue,
             OrderEventParserInterface<MemoryPool>& parser,
             OrderWatcherInterface<MemoryPool>& watcher)
        : strand_(boost::asio::make_strand(thread_pool)),
          queue_(queue),
          parser_(parser),
          watcher_(watcher) {}
    void OnDataEnqueued() override {
        boost::asio::co_spawn(strand_, ProcessQueue(), boost::asio::detached);
    }
    ~Listener() override = default;

  private:
    boost::asio::awaitable<void> ProcessQueue() {
        std::vector<char> msg;
        simdjson::ondemand::parser parser;
        while (queue_.try_dequeue(msg)) {
            // Обработка данных (например, parse via simdjson)
            logi("process message of size {}", msg.size());
            simdjson::padded_string padded_json(msg.data(), msg.size());
            simdjson::ondemand::document doc;
            simdjson::error_code error = parser.iterate(padded_json).get(doc);
            if (error) {
                logi("parsing error: {}", simdjson::error_message(error));
                co_return;
            }
            auto [status, ptr] = parser_.ParseAndCreate(doc);
            if (!status) co_return;
            watcher_.OnEvent(ptr);
            std::string str(msg.data(), msg.size());
            logi("parsed JSON: {}", str);
        }
        co_return;
    }
};
};  // namespace order_response
};  // namespace impl
};  // namespace bybit
};  // namespace aoe