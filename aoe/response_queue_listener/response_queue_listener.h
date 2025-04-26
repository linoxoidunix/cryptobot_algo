#pragma once
#include "aoe/response_queue_listener/i_response_queue_listener.h"
#include "aot/Logger.h"
#include "boost/asio.hpp"
#include "concurrentqueue.h"
#include "simdjson.h"

namespace aoe {
namespace impl {
class ResponseQueueListener : public ResponseQueueListenerInterface {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    moodycamel::ConcurrentQueue<std::vector<char>>& queue_;

  public:
    ResponseQueueListener(boost::asio::thread_pool& thread_pool,
                          moodycamel::ConcurrentQueue<std::vector<char>>& queue)
        : strand_(boost::asio::make_strand(thread_pool)), queue_(queue) {}
    void OnDataEnqueued() override {
        boost::asio::co_spawn(strand_, ProcessQueue(), boost::asio::detached);
    }

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
            std::string str(msg.data(), msg.size());
            logi("parsed JSON: {}", str);
        }
        co_return;
    }
};
};  // namespace impl
};  // namespace aoe