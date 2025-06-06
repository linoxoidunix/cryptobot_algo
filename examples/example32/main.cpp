#include <thread>

#include "aoe/aoe.h"
#include "aoe/binance/auth/web_socket/auth.h"
#include "aoe/binance/credentials_loader/credentials_loader.h"
#include "aoe/binance/session/web_socket/web_socket.h"
#include "aoe/binance/session_setup/web_socket/private/session_setup.h"
#include "aoe/response_queue_listener/response_queue_listener.h"
#include "aoe/signer/ed25519/signer.h"
#include "aos/aos.h"
#include "fmtlog.h"
#include "aos/logger/logger.h"


int main(int argc, char** argv) {
    {
        boost::asio::thread_pool thread_pool;
        LogPolling log_polling(thread_pool, std::chrono::microseconds(1));

        std::string config_path = argv[1];
        aoe::binance::impl::CredentialsLoader binance_credentials(config_path);
        //-------------------------------------------------------------------------------
        boost::asio::io_context ioc;
        moodycamel::ConcurrentQueue<std::vector<char>> response_queue_;
        aoe::impl::ResponseQueueListener listener(thread_pool, response_queue_);
        aoe::binance::impl::main_net::private_channel::SessionRW ws(
            ioc, response_queue_, listener);
        //-------------------------------
        aoe::binance::impl::main_net::PrivateSessionSetup
            private_session_setuper(ws, binance_credentials);
        auto auth_success = private_session_setuper.Setup();
        if (!auth_success) return 0;
        auto thread_ = std::jthread([&ioc]() { ioc.run(); });
    }
    fmtlog::poll();
    return 0;
}
