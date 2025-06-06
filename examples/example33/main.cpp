#include <thread>

#include "aoe/aoe.h"
#include "aoe/binance/credentials_loader/credentials_loader.h"
#include "aoe/binance/request/get_snapshot/request.h"
#include "aoe/binance/rest_request_sender/rest_request_sender.h"
#include "aoe/binance/session/rest/session.h"
#include "aoe/response_queue_listener/response_queue_listener.h"
#include "aos/aos.h"
#include "fmtlog.h"
#include "aos/common/mem_pool.h"

int main(int argc, char** argv) {
    {
        boost::asio::thread_pool thread_pool;
        boost::asio::io_context ioc;
        moodycamel::ConcurrentQueue<std::vector<char>> response_queue_;
        aos::OrderBookEventListener<
            double, double, common::MemoryPoolThreadSafety,
            std::unordered_map<double, aos::OrderBookLevel<double, double>*>>
            order_book{thread_pool, 1000};
        aoe::binance::impl::main_net::spot::OrderBookSync<
            double, double, common::MemoryPoolThreadSafety,
            std::unordered_map<double, aos::OrderBookLevel<double, double>*>>
            order_book_sync{thread_pool, order_book};
        aoe::binance::impl::SnapshotEventParser<double, double,
                                                common::MemoryPoolThreadSafety>
            snapshot_parser(100);
        aoe::binance::impl::snapshot::spot::Listener listener(
            thread_pool, response_queue_, snapshot_parser, order_book_sync);
        aoe::binance::impl::main_net::spot::RestSessionRW session(
            ioc, response_queue_, listener);
        aoe::binance::snapshot::impl::SnapshotRequestSender<
            common::MemoryPoolThreadSafety>
            sender(session);
        aoe::binance::snapshot::main_net::spot::impl::Request<
            common::MemoryPoolThreadSafety>
            request;
        request.SetTradingPair(aos::TradingPair::kBTCUSDT);
        request.SetLimit(1);

        sender.Send(request);
        auto thread_ = std::jthread([&ioc]() { ioc.run(); });
        // std::this_thread::sleep_for(std::chrono::seconds(1000));
    }
    fmtlog::poll();
    return 0;
}
