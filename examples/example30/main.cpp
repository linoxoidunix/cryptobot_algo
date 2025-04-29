// #include "aoe/bybit/order_manager/order_manager.h"
// #include "aoe/bybit/order_storage/order_storage.h"
// #include "aoe/bybit/request/amend_order/request.h"
// #include "aoe/bybit/request/cancel_order/request.h"
// #include "aoe/bybit/request/place_order/request.h"
// #include "aoe/bybit/request_maker/for_web_socket/place_order/request_maker.h"
// #include "aoe/session/web_socket/web_socket.h"
// #include "aoe/session_provider/web_socket/web_socket_session_provider.h"
// #include "aos/multi_order_manager/multi_order_manager.h"
#include "aoe/aoe.h"
#include "aos/aos.h"
#include "aos/trading_pair_printer/trading_pair_printer.h"
#include "aot/common/mem_pool.h"

int main(int argc, char** argv) {
    {
        boost::asio::thread_pool thread_pool;
        LogPolling log_polling(thread_pool, std::chrono::microseconds(1));

        //-------------------------------------------------------------------------------

        //--------------
        std::unique_ptr<aoe::bybit::WebSocketPrivateSessionInterface> session;
        aoe::impl::WebSocketSessionProvider<common::MemoryPoolThreadSafety>
            wss_provider(*session);

        aoe::bybit::place_order::RequestMaker<common::MemoryPoolThreadSafety>
            place_order_maker;
        aoe::bybit::cancel_order::RequestMaker<common::MemoryPoolThreadSafety>
            cancel_order_maker;
        aoe::bybit::amend_order::RequestMaker<common::MemoryPoolThreadSafety>
            amend_order_maker;
        aos::impl::NumberPoolDefault<uint64_t> number_pool;
        aos::impl::UIDGeneratorDefault<uint64_t> uid_generator;
        aos::impl::UIDManagerDefault<uint64_t> uid_manager(uid_generator,
                                                           number_pool);

        aoe::bybit::impl::external::ws::SingleOrderAPI<
            common::MemoryPoolThreadSafety>
            bybit_api(wss_provider, place_order_maker, cancel_order_maker,
                      amend_order_maker);
        aos::impl::MultiOrderManagerDefault<common::MemoryPoolThreadSafety>
            multi_order_manager;
        aoe::bybit::impl::OrderStorage order_storage;
        auto ptr = std::make_unique<
            aoe::bybit::impl::OrderManager<common::MemoryPoolThreadSafety>>(
            order_storage, bybit_api, uid_manager);
        aoe::bybit::impl::OrderWatcher<common::MemoryPoolThreadSafety>
            order_watcher(*ptr);
        multi_order_manager.Register(common::ExchangeId::kBybit,
                                     std::move(ptr));
        //--------------
        moodycamel::ConcurrentQueue<std::vector<char>> response_queue_;
        aos::impl::TradingPairFactoryTest trading_pair_factory;
        aoe::bybit::impl::OrderEventParser<common::MemoryPoolThreadSafety>
            order_event_parser(100, trading_pair_factory);
        aoe::bybit::impl::order_response::Listener listener(
            thread_pool, response_queue_, order_event_parser, order_watcher);

                boost::asio::io_context ioc;
        session = std::make_unique<
            aoe::bybit::impl::test_net::private_channel::Session>(
            ioc, response_queue_, listener);
        //-------------------------------
        boost::asio::steady_timer timer(ioc);
        aoe::bybit::impl::private_channel::PingManager<std::chrono::seconds>
            ping_manager(timer, *session.get(), std::chrono::seconds(20));
        std::string config_path = argv[1];
        aoe::bybit::impl::CredentialsLoader bybit_credentials(config_path);
        aoe::bybit::impl::test_net::PrivateSessionSetup private_session_setuper(
            *session.get(), bybit_credentials, ping_manager);
        private_session_setuper.Setup();
        //-------------------------------
        aoe::bybit::impl::OrderSubscriptionBuilder order_subscription_builder(
            *session.get());
        order_subscription_builder.Subscribe();
        std::thread thread_ioc([&ioc]() { ioc.run(); });
    }
    fmtlog::poll();
    return 0;
}