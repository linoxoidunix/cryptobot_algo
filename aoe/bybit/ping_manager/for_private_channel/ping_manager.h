#pragma once
#include "aoe/bybit/session/web_socket/i_web_socket.h"
#include "aoe/ping_manager/i_ping_manager.h"
#include "nlohmann/json.hpp"
namespace aoe {
namespace bybit {
namespace impl {
namespace private_channel {
template <typename DurationType>
class PingManager : public PingManagerInterface {
    boost::asio::steady_timer& timer_;
    WebSocketPrivateSessionWInterface& ws_;
    DurationType interval_;

  public:
    PingManager(boost::asio::steady_timer& timer,
                WebSocketPrivateSessionWInterface& ws, DurationType interval)
        : timer_(timer), ws_(ws), interval_(interval) {}

    void Start() override {
        timer_.expires_after(interval_);
        timer_.async_wait([this](const boost::system::error_code& ec) {
            if (!ec) {
                SendPing();
            }
        });
    }

  private:
    void SendPing() {
        nlohmann::json j;
        j["op"] = "ping";
        ws_.AsyncWrite(std::move(j));

        // Перезапускаем
        Start();
    }
};
};  // namespace private_channel
};  // namespace impl
};  // namespace bybit
};  // namespace aoe