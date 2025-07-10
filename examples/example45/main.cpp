#include <thread>

#include "aoe/bybit/events_acceptor/funding_rate_event/acceptor.h"
#include "aoe/bybit/response_queue_listener/json/ws/tickers_response/funding_rate/listener.h"
#include "aoe/bybit/session/web_socket/web_socket.h"
#include "aoe/bybit/subscription_builder/tickers/subscription_builder.h"
#include "aos/common/mem_pool.h"
#include "aos/logger/logger.h"
#include "aos/order_book/order_book.h"
#include "aos/order_book_level/order_book_level.h"
#include "concurrentqueue.h"

int main(int, char**) {
    try {
        boost::asio::thread_pool thread_pool;
        LogPolling log_polling(thread_pool, std::chrono::microseconds(1));
        boost::asio::io_context ioc;
        //-------------------------------------------------------------------------------
        moodycamel::ConcurrentQueue<std::vector<char>> queue;
        aoe::bybit::impl::event_acceptor::funding_rate_event::Acceptor<
            common::MemoryPoolThreadSafety>
            acceptor(thread_pool);
        aoe::bybit::impl::ticker_response::funding_rate::Listener<
            common::MemoryPoolThreadSafety>
            listener(thread_pool, queue, acceptor);
        aoe::bybit::impl::main_net::linear::tickers_channel::SessionRW session(
            ioc, queue, listener);
        aoe::bybit::impl::linear::TickersSubscriptionBuilder subscription(
            session, aos::TradingPair::kBTCUSDT);
        subscription.Subscribe();
        session.StartAsync();
        std::jthread([ioc_ptr = &ioc]() { ioc_ptr->run(); });
    } catch (...) {
        loge("error occured");
    }
    fmtlog::poll();
    return 0;
}