#include <thread>

#include "aoe/aoe.h"
#include "aoe/bybit/order_book_sync/order_book_sync.h"
#include "aoe/bybit/parser/json/ws/order_book_response/parser.h"
#include "aos/aos.h"
#include "aos/common/mem_pool.h"
#include "aos/logger/logger.h"

int main() {
    try {
        boost::asio::thread_pool thread_pool;
        LogPolling log_polling(thread_pool, std::chrono::microseconds(1));

        //-------------------------------------------------------------------------------
        moodycamel::ConcurrentQueue<std::vector<char>> queue;
        aos::OrderBookEventListener<
            double, double, common::MemoryPoolThreadSafety,
            std::unordered_map<double, aos::OrderBookLevel<double, double>*>>
            order_book{thread_pool, 1000};
        aoe::bybit::impl::OrderBookSync<
            double, double, common::MemoryPoolThreadSafety,
            std::unordered_map<double, aos::OrderBookLevel<double, double>*>>
            order_book_sync{thread_pool, order_book};
        aoe::bybit::impl::OrderBookEventParser<double, double,
                                               common::MemoryPoolThreadSafety>
            parser{1000};
        aoe::bybit::impl::order_book_response::Listener<
            double, double, common::MemoryPoolThreadSafety>
            listener(thread_pool, queue, order_book_sync);

        //-------------------------------------------------------------------------------

        boost::asio::io_context ioc_order_book_channel;
        aoe::bybit::impl::main_net::spot::order_book_channel::SessionRW
            session_trade_channel(ioc_order_book_channel, queue, listener);
        aoe::bybit::impl::spot::OrderBookSubscriptionBuilder sub_builder{
            session_trade_channel, aoe::bybit::spot::Depth::k50,
            aos::TradingPair::kBTCUSDT};
        sub_builder.Subscribe();
        auto thread = std::jthread(
            [&ioc_order_book_channel]() { ioc_order_book_channel.run(); });
    } catch (...) {
        loge("error occured");
    }
    fmtlog::poll();
    return 0;
}