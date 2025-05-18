#include <thread>

#include "aoe/aoe.h"
#include "aoe/binance/credentials_loader/credentials_loader.h"
#include "aoe/binance/request/get_snapshot/request.h"
#include "aoe/binance/rest_request_sender/rest_request_sender.h"
#include "aoe/binance/session/rest/session.h"
#include "aoe/response_queue_listener/response_queue_listener.h"
#include "aos/aos.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"

int main(int argc, char** argv) {
    {
        boost::asio::thread_pool thread_pool;
        boost::asio::io_context ioc;
        moodycamel::ConcurrentQueue<std::vector<char>> response_queue_;
        aoe::impl::ResponseQueueListener listener(thread_pool, response_queue_);
        aoe::binance::impl::main_net::spot::RestSessionRW session(
            ioc, response_queue_, listener);
        aoe::binance::snapshot::impl::SnapshotRequestSender<
            common::MemoryPoolThreadSafety>
            sender(session);
        aoe::binance::snapshot::main_net::spot::impl::Request<
            common::MemoryPoolThreadSafety>
            request;
        request.SetTradingPair(aos::TradingPair::kBTCUSDT);
        sender.Send(request);
        auto thread_ = std::jthread([&ioc]() { ioc.run(); });
        // std::this_thread::sleep_for(std::chrono::seconds(15));
    }
    fmtlog::poll();
    return 0;
}
