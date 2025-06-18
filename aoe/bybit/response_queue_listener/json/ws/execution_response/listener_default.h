#pragma once
#include "aoe/bybit/execution_watcher/i_execution_watcher.h"
#include "aoe/bybit/parser/json/ws/execution_response/execution_event_parser.h"
#include "aoe/bybit/response_queue_listener/json/ws/execution_response/listener.h"
#include "aoe/response_queue_listener/i_response_queue_listener.h"
#include "aos/converters/big_string_view_to_trading_pair/big_string_view_to_trading_pair.h"
#include "aos/logger/mylog.h"
#include "boost/asio.hpp"
#include "concurrentqueue.h"
#include "simdjson.h"  // NOLINT

namespace aoe {
namespace bybit {
namespace impl {
namespace execution_response {

template <template <typename> typename MemoryPool, typename PositionT>
class ListenerDefault : public ResponseQueueListenerInterface {
    ExecutionEventParser<MemoryPool, PositionT> parser_;
    Listener<MemoryPool, PositionT> listener_;

  public:
    ListenerDefault(boost::asio::thread_pool& thread_pool,
                    moodycamel::ConcurrentQueue<std::vector<char>>& queue,
                    ExecutionWatcherInterface<MemoryPool, PositionT>& watcher,
                    size_t number_events)
        : parser_(number_events),
          listener_(thread_pool, queue, parser_, watcher) {}
    void OnDataEnqueued() override { listener_.OnDataEnqueued(); }
    ~ListenerDefault() override = default;
};
};  // namespace execution_response
};  // namespace impl
};  // namespace bybit
};  // namespace aoe