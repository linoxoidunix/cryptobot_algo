#pragma once
#include "aoe/bybit/order_watcher/i_order_watcher.h"
#include "aoe/bybit/parser/json/ws/order_response/order_event_parser.h"
#include "aoe/bybit/response_queue_listener/json/ws/order_response/listener.h"
#include "aoe/response_queue_listener/i_response_queue_listener.h"
#include "aos/trading_pair_factory/trading_pair_factory.h"
#include "aot/Logger.h"
#include "boost/asio.hpp"
#include "concurrentqueue.h"
#include "simdjson.h"

namespace aoe {
namespace bybit {
namespace impl {
namespace order_response {

template <template <typename> typename MemoryPool>
class ListenerDefault : public ResponseQueueListenerInterface {
    aos::impl::TradingPairFactoryTest trading_pair_factory_;
    OrderEventParser<MemoryPool> order_event_parser_;
    Listener<MemoryPool> listener_;

  public:
    ListenerDefault(boost::asio::thread_pool& thread_pool,
                    moodycamel::ConcurrentQueue<std::vector<char>>& queue,
                    OrderWatcherInterface<MemoryPool>& watcher,
                    size_t number_events)
        : order_event_parser_(number_events, trading_pair_factory_),
          listener_(thread_pool, queue, order_event_parser_, watcher) {}
    void OnDataEnqueued() override { listener_.OnDataEnqueued(); }
    ~ListenerDefault() override = default;
};
};  // namespace order_response
};  // namespace impl
};  // namespace bybit
};  // namespace aoe