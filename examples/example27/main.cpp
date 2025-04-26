#include <thread>

#include "aoe/aoe.h"
#include "aoe/bybit/auth/web_socket/auth.h"
#include "aoe/bybit/credentials_loader/credentials_loader.h"
#include "aoe/response_queue_listener/response_queue_listener.h"
#include "aoe/signer/hmac_sha256/signer.h"
#include "aos/aos.h"
#include "aot/Logger.h"
void SendSubscribePeriodically(boost::asio::steady_timer& timer,
                               aoe::impl::WebSocketSession& ws) {
    nlohmann::json j;
    j["op"] = "ping";
    ws.AsyncWrite(std::move(j));

    // перенастроить таймер на следующие 20 секунд
    timer.expires_after(std::chrono::seconds(20));
    timer.async_wait([&timer, &ws](const boost::system::error_code& ec) {
        if (!ec) {
            SendSubscribePeriodically(timer, ws);
        }
    });
}

int main(int argc, char** argv) {
    {
        boost::asio::thread_pool thread_pool;
        LogPolling log_polling(thread_pool, std::chrono::microseconds(1));
        boost::asio::ssl::context ctx_{
            boost::asio::ssl::context::tlsv12_client};
        boost::asio::io_context ioc;
        moodycamel::ConcurrentQueue<std::vector<char>> response_queue_;
        aoe::impl::ResponseQueueListener listener(thread_pool, response_queue_);
        aoe::impl::WebSocketSession ws(ioc, ctx_, "stream.bybit.com", "443",
                                       "/v5/private", response_queue_,
                                       listener);
        //-------------------------------------------------------------------------------
        std::string config_path = argv[1];
        aoe::bybit::impl::CredentialsLoader bybit_credentials(config_path);
        auto [status_api_key, api_key_mainnet] =
            bybit_credentials.ApiKeyMainNet();
        if (!status_api_key) {
            return 0;
        }
        auto [status_secert_key, secret_key_mainnet] =
            bybit_credentials.SecretKeyMainNet();
        if (!status_secert_key) {
            return 0;
        }
        aoe::impl::StaticApiKey api_key(api_key_mainnet);
        aoe::impl::StaticSecretKey secret_key(secret_key_mainnet);
        aoe::hmac_sha256::impl::Signer signer(api_key, secret_key);
        aoe::bybit::impl::Authentificator auth(ws, signer);
        auth.Auth();
        //-------------------------------------------------------------------------------
        //  Создаём таймер
        boost::asio::steady_timer timer(ioc);
        timer.expires_after(std::chrono::seconds(20));  // сразу отправить
        timer.async_wait([&timer, &ws](const boost::system::error_code& ec) {
            if (!ec) {
                SendSubscribePeriodically(timer, ws);
            }
        });

        nlohmann::json j;
        j["op"]   = "subscribe";
        j["args"] = {"execution"};
        ws.AsyncWrite(std::move(j));
        std::thread thread_ioc_bybit([&ioc]() { ioc.run(); });
        thread_ioc_bybit.join();
    }
    return 0;
}