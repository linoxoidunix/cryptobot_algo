#include <thread>

#include "aoe/binance/credentials_loader/credentials_loader.h"
#include "aoe/binance/order_book_sync/order_book_sync.h"
#include "aoe/binance/parser/json/rest/snapshot_response/parser.h"
#include "aoe/binance/request/get_snapshot/request.h"
#include "aoe/binance/rest_request_sender/rest_request_sender.h"
#include "aoe/binance/session/rest/session.h"
#include "aoe/response_queue_listener/response_queue_listener.h"
#include "aos/common/mem_pool.h"
#include "aos/logger/mylog.h"
#include "aos/order_book/order_book.h"

int main() {
    {
        try {
            boost::asio::thread_pool thread_pool;
            boost::asio::io_context ioc;

            aoe::binance::impl::SnapshotEventParser<
                double, double, common::MemoryPoolThreadSafety>
                snapshot_parser(100);

            moodycamel::ConcurrentQueue<std::vector<char>> response_queue;
            aos::OrderBookEventListener<
                double, double, common::MemoryPoolThreadSafety,
                std::unordered_map<double,
                                   aos::OrderBookLevel<double, double>*>>
                order_book{thread_pool, 1000};
            aoe::binance::impl::main_net::spot::OrderBookSync<
                double, double, common::MemoryPoolThreadSafety,
                std::unordered_map<double,
                                   aos::OrderBookLevel<double, double>*>>
                order_book_sync{thread_pool, order_book};

            aoe::binance::impl::snapshot::spot::Listener listener(
                thread_pool, response_queue, snapshot_parser, order_book_sync);
            aoe::binance::impl::main_net::spot::RestSessionRW session(
                ioc, response_queue, listener);
            aoe::binance::snapshot::impl::SnapshotRequestSender<
                common::MemoryPoolThreadSafety>
                sender(session);
            aoe::binance::snapshot::main_net::spot::impl::Request<
                common::MemoryPoolThreadSafety>
                request;
            request.SetTradingPair(aos::TradingPair::kBTCUSDT);
            request.SetLimit(1);
            sender.Send(request);
            session.Start();
            auto thread = std::jthread([&ioc]() {
                ioc.run();
                logi("ioc finished");
            });
            std::this_thread::sleep_for(std::chrono::seconds(2));
        } catch (...) {
            loge("error occured");
        }
    }
    fmtlog::poll();
    return 0;
}
