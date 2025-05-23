// #include "aoe/bybit/order_manager/order_manager.h"
// #include "aoe/bybit/order_storage/order_storage.h"
// #include "aoe/bybit/request/amend_order/request.h"
// #include "aoe/bybit/request/cancel_order/request.h"
// #include "aoe/bybit/request/place_order/request.h"
// #include "aoe/bybit/request_maker/for_web_socket/place_order/request_maker.h"
// #include "aoe/session/web_socket/web_socket.h"
// #include "aoe/session_provider/web_socket/web_socket_session_provider.h"
// #include "aos/multi_order_manager/multi_order_manager.h"
#include <thread>

#include "aoe/aoe.h"
#include "aoe/bybit/order_book_sync/order_book_sync.h"
#include "aoe/bybit/parser/json/ws/order_book_response/parser.h"
#include "aos/aos.h"
#include "aot/common/mem_pool.h"

int main(int argc, char** argv) {
    {
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
            listener(thread_pool, queue, parser, order_book_sync);

        //-------------------------------------------------------------------------------

        boost::asio::io_context ioc_order_book_channel;
        aoe::bybit::impl::main_net::spot::order_book_channel::SessionRW
            session_trade_channel(ioc_order_book_channel, queue, listener);
        aoe::bybit::impl::spot::OrderBookSubscriptionBuilder sub_builder{
            session_trade_channel, aoe::bybit::spot::Depth::k50,
            aos::TradingPair::kBTCUSDT};
        sub_builder.Subscribe();
        auto thread_ = std::jthread(
            [&ioc_order_book_channel]() { ioc_order_book_channel.run(); });
    }
    fmtlog::poll();
    return 0;
}