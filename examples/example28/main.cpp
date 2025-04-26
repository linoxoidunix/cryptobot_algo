#include <thread>

#include "aoe/aoe.h"
#include "aoe/bybit/auth/web_socket/auth.h"
#include "aoe/bybit/credentials_loader/credentials_loader.h"
#include "aoe/bybit/ping_manager/for_private_channel/ping_manager.h"
#include "aoe/bybit/session/web_socket/web_socket.h"
#include "aoe/bybit/session_setup/web_socket/private/session_setup.h"
#include "aoe/bybit/subscription_builder/subscription_builder.h"
#include "aoe/response_queue_listener/response_queue_listener.h"
#include "aoe/signer/hmac_sha256/signer.h"
#include "aos/aos.h"
#include "aot/Logger.h"

int main(int argc, char** argv) {
    {
        boost::asio::thread_pool thread_pool;
        LogPolling log_polling(thread_pool, std::chrono::microseconds(1));

        std::string config_path = argv[1];
        aoe::bybit::impl::CredentialsLoader bybit_credentials(config_path);
        //-------------------------------------------------------------------------------
        // boost::asio::io_context ioc;
        // moodycamel::ConcurrentQueue<std::vector<char>> response_queue_;
        // aoe::impl::ResponseQueueListener listener(thread_pool,
        // response_queue_);
        // aoe::bybit::impl::test_net::private_channel::Session ws(
        //     ioc, response_queue_, listener);
        // //-------------------------------
        // boost::asio::steady_timer timer(ioc);
        // aoe::bybit::impl::private_channel::PingManager<std::chrono::seconds>
        //     ping_manager(timer, ws, std::chrono::seconds(20));
        // aoe::bybit::impl::test_net::PrivateSessionSetup
        // private_session_setuper(
        //     ws, bybit_credentials, ping_manager);
        // private_session_setuper.Setup();
        // //-------------------------------
        // aoe::bybit::impl::ExecutionSubscriptionBuilder execution_sub_builder(
        //     ws);
        // execution_sub_builder.Subscribe();
        // std::thread thread_ioc([&ioc]() { ioc.run(); });
        //-------------------------------------------------------------------------------
        boost::asio::io_context ioc_order;
        moodycamel::ConcurrentQueue<std::vector<char>> response_queue_order_;
        aoe::impl::ResponseQueueListener listener_order(thread_pool,
                                                        response_queue_order_);
        aoe::bybit::impl::test_net::private_channel::Session ws_order(
            ioc_order, response_queue_order_, listener_order);
        //-------------------------------
        boost::asio::steady_timer timer_order(ioc_order);
        aoe::bybit::impl::private_channel::PingManager<std::chrono::seconds>
            ping_manager_order(timer_order, ws_order, std::chrono::seconds(20));
        aoe::bybit::impl::test_net::PrivateSessionSetup
            private_session_setuper_order(ws_order, bybit_credentials,
                                          ping_manager_order);
        private_session_setuper_order.Setup();
        //-------------------------------
        aoe::bybit::impl::OrderSubscriptionBuilder order_sub_builder(ws_order);
        order_sub_builder.Subscribe();
        std::thread thread_ioc_order([&ioc_order]() { ioc_order.run(); });
        //-------------------------------------------------------------------------------
        // thread_ioc.join();
        thread_ioc_order.join();
    }
    return 0;
}