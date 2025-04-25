#include <thread>

#include "aoe/aoe.h"
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

int main() {
    {
        boost::asio::thread_pool thread_pool;
        LogPolling log_polling(thread_pool, std::chrono::microseconds(1));
        boost::asio::ssl::context ctx_{
            boost::asio::ssl::context::tlsv12_client};
        boost::asio::io_context ioc;
        aoe::impl::WebSocketSession ws(ioc, ctx_, "stream.bybit.com", "443",
                                       "/v5/public/linear");
        // Создаём таймер
        boost::asio::steady_timer timer(ioc);
        timer.expires_after(std::chrono::seconds(0));  // сразу отправить
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