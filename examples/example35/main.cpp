#include <thread>

#include "aoe/binance/order_book_sync/order_book_sync.h"
#include "aoe/binance/parser/json/ws/diff_response/parser.h"
#include "aoe/binance/response_queue_listener/json/ws/diff_response/listener.h"
#include "aoe/binance/session/web_socket/web_socket.h"
#include "aoe/binance/subscription_builder/subscription_builder.h"
#include "aos/common/mem_pool.h"
#include "aos/logger/logger.h"
#include "aos/order_book/order_book.h"
#include "aos/order_book_level/order_book_level.h"
#include "aos/order_book_subscriber/order_book_subscribers.h"

int main() {
    {
        try {
            fmtlog::setLogLevel(fmtlog::LogLevel::DBG);
            logd("{}", aos::TradingPair::kBTCUSDT);

            boost::asio::thread_pool thread_pool;
            LogPolling log_polling(thread_pool, std::chrono::microseconds(1));

            //-------------------------------------------------------------------------------
            moodycamel::ConcurrentQueue<std::vector<char>> queue;
            aos::OrderBookEventListener<
                double, double, common::MemoryPoolThreadSafety,
                std::unordered_map<double,
                                   aos::OrderBookLevel<double, double>*>>
                order_book{thread_pool, 1000};
            aos::OrderBookNotifier<double, double,
                                   common::MemoryPoolThreadSafety>
                order_book_notifier{order_book};
            aoe::binance::impl::main_net::spot::OrderBookSync<
                double, double, common::MemoryPoolThreadSafety,
                std::unordered_map<double,
                                   aos::OrderBookLevel<double, double>*>>
                order_book_sync{thread_pool, order_book_notifier};
            aoe::binance::impl::diff_response::spot::Listener<
                double, double, common::MemoryPoolThreadSafety>
                listener(thread_pool, queue, order_book_sync);
            //-------------------------------------------------------------------------------
            aos::impl::BestBidOrBestAskNotifier<double, double>
                best_bid_or_best_ask_notifier{thread_pool, order_book};
            aos::impl::BestBidNotifier<double, double> best_bid_notifier{
                thread_pool, order_book};
            aos::impl::BestAskNotifier<double, double> best_ask_notifier{
                thread_pool, order_book};
            order_book_notifier.AddSubscriber(best_bid_or_best_ask_notifier);
            order_book_notifier.AddSubscriber(best_bid_notifier);
            order_book_notifier.AddSubscriber(best_ask_notifier);

            //-------------------------------------------------------------------------------

            boost::asio::io_context ioc_order_book_channel;
            aoe::binance::impl::main_net::spot::order_book_channel::SessionRW
                session_trade_channel(ioc_order_book_channel, queue, listener);
            aoe::binance::impl::spot::DiffSubscriptionBuilder sub_builder{
                session_trade_channel,
                aoe::binance::spot::DiffUpdateSpeed_ms::k100,
                aos::TradingPair::kBTCUSDT};
            sub_builder.Subscribe();
            auto thread = std::jthread(
                [&ioc_order_book_channel]() { ioc_order_book_channel.run(); });
        } catch (...) {
            loge("error occured");
        }
    }
    fmtlog::poll();
    return 0;
}