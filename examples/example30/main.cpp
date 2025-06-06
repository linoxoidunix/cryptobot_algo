// #include "aoe/bybit/order_manager/order_manager.h"
// #include "aoe/bybit/order_storage/order_storage.h"
// #include "aoe/bybit/request/amend_order/request.h"
// #include "aoe/bybit/request/cancel_order/request.h"
// #include "aoe/bybit/request/place_order/request.h"
// #include "aoe/bybit/request_maker/for_web_socket/place_order/request_maker.h"
// #include "aoe/session/web_socket/web_socket.h"
// #include "aoe/session_provider/web_socket/web_socket_session_provider.h"
// #include "aos/multi_order_manager/multi_order_manager.h"
#include <thread>

#include "aoe/aoe.h"
#include "aoe/bybit/execution_watcher/execution_watcher.h"
#include "aoe/bybit/response_queue_listener/json/ws/execution_response/listener_default.h"
#include "aoe/bybit/response_queue_listener/json/ws/order_response/listener_default.h"
#include "aos/aos.h"
#include "aos/position/position.h"
#include "aos/position_storage/position_storage_by_pair/position_storage_by_pair_default.h"
#include "aos/common/mem_pool.h"
#include "aos/logger/logger.h"


class BybitOrderManagerListener {
  public:
    using WebSocketSessionW = aoe::bybit::WebSocketPrivateSessionWInterface;
    using ResponseQueue     = moodycamel::ConcurrentQueue<std::vector<char>>;

  private:
    boost::asio::thread_pool& thread_pool_;
    WebSocketSessionW& session_ref_;

    aoe::impl::WebSocketSessionProvider<common::MemoryPoolThreadSafety>
        wss_provider_;
    aoe::bybit::impl::external::ws::SingleOrderAPIDefault<
        common::MemoryPoolThreadSafety>
        bybit_api_;
    aos::impl::MultiOrderManagerDefault<common::MemoryPoolThreadSafety>
        multi_order_manager_;
    std::unique_ptr<
        aoe::bybit::impl::OrderManagerDefault<common::MemoryPoolThreadSafety>>
        order_manager_;
    aoe::bybit::impl::OrderWatcher<common::MemoryPoolThreadSafety>
        order_watcher_;

    ResponseQueue response_queue_;
    aoe::bybit::impl::order_response::ListenerDefault<
        common::MemoryPoolThreadSafety>
        listener_;

  public:
    BybitOrderManagerListener(boost::asio::thread_pool& thread_pool,
                              WebSocketSessionW& session)
        : thread_pool_(thread_pool),
          session_ref_(session),
          wss_provider_(session_ref_),
          bybit_api_(wss_provider_),
          order_manager_(std::make_unique<aoe::bybit::impl::OrderManagerDefault<
                             common::MemoryPoolThreadSafety>>(bybit_api_)),
          order_watcher_(*order_manager_),
          listener_(thread_pool_, response_queue_, order_watcher_, 100) {
        multi_order_manager_.Register(common::ExchangeId::kBybit,
                                      std::move(order_manager_));
    }
    aoe::ResponseQueueListenerInterface& GetListener() { return listener_; }
    ResponseQueue& GetQueue() { return response_queue_; }
};

template <typename Price = double, typename Qty = double,
          typename PositionT = aos::impl::NetPositionDefault<Price, Qty>>
class BybitNetPositionManagerListener {
    using ResponseQueue = moodycamel::ConcurrentQueue<std::vector<char>>;

  private:
    boost::asio::thread_pool& thread_pool_;
    ResponseQueue response_queue_;
    aos::impl::NetPositionStorageByPairDefault<> position_storage_;
    aoe::bybit::impl::ExecutionWatcherDefault<common::MemoryPoolThreadSafety,
                                              PositionT, Price, Qty>
        execution_watcher_;

    aoe::bybit::impl::execution_response::ListenerDefault<
        common::MemoryPoolThreadSafety, PositionT>
        listener_;

  public:
    BybitNetPositionManagerListener(boost::asio::thread_pool& thread_pool)
        : thread_pool_(thread_pool),
          execution_watcher_(position_storage_),
          listener_(thread_pool_, response_queue_, execution_watcher_, 100) {}
    aoe::ResponseQueueListenerInterface& GetListener() { return listener_; }
    ResponseQueue& GetQueue() { return response_queue_; }
};

class OrderChannelSessionStarter {
    boost::asio::io_context& context_;
    boost::asio::steady_timer timer_;
    aoe::bybit::WebSocketPrivateSessionRWInterface& session_;
    aoe::bybit::impl::private_channel::PingManager<std::chrono::seconds>
        ping_manager_;
    aoe::bybit::impl::CredentialsLoader bybit_credentials_;
    aoe::bybit::impl::test_net::PrivateSessionSetup private_session_setuper_;
    aoe::bybit::impl::OrderSubscriptionBuilder order_subscription_builder_;
    std::jthread thread_;

  public:
    OrderChannelSessionStarter(
        boost::asio::io_context& context,
        aoe::bybit::WebSocketPrivateSessionRWInterface& session,
        std::string config_path)
        : context_(context),
          timer_(context_),
          session_(session),
          ping_manager_(timer_, session_, std::chrono::seconds(20)),
          bybit_credentials_(config_path),
          private_session_setuper_(session_, bybit_credentials_, ping_manager_),
          order_subscription_builder_(session_) {}
    void Run() {
        auto status = private_session_setuper_.Setup();
        if (!status) return;
        order_subscription_builder_.Subscribe();
        thread_ = std::jthread([this]() { context_.run(); });
    }
};

class ExecutionChannelSessionStarter {
    boost::asio::io_context& context_;
    boost::asio::steady_timer timer_;
    aoe::bybit::WebSocketPrivateSessionRWInterface& session_;
    aoe::bybit::impl::private_channel::PingManager<std::chrono::seconds>
        ping_manager_;
    aoe::bybit::impl::CredentialsLoader bybit_credentials_;
    aoe::bybit::impl::test_net::PrivateSessionSetup private_session_setuper_;
    aoe::bybit::impl::ExecutionSubscriptionBuilder
        execution_subscription_builder_;
    std::jthread thread_;

  public:
    ExecutionChannelSessionStarter(
        boost::asio::io_context& context,
        aoe::bybit::WebSocketPrivateSessionRWInterface& session,
        std::string config_path)
        : context_(context),
          timer_(context_),
          session_(session),
          ping_manager_(timer_, session_, std::chrono::seconds(20)),
          bybit_credentials_(config_path),
          private_session_setuper_(session_, bybit_credentials_, ping_manager_),
          execution_subscription_builder_(session_) {
        private_session_setuper_.Setup();
        execution_subscription_builder_.Subscribe();
    }
    void Run() {
        thread_ = std::jthread([this]() { context_.run(); });
    }
};

int main(int argc, char** argv) {
    {
        boost::asio::thread_pool thread_pool;
        LogPolling log_polling(thread_pool, std::chrono::microseconds(1));

        //-------------------------------------------------------------------------------
        boost::asio::io_context ioc_trade_channel;
        aoe::bybit::impl::test_net::trade_channel::SessionW
            session_trade_channel(ioc_trade_channel);
        BybitOrderManagerListener order_manager(thread_pool,
                                                session_trade_channel);
        //-------------------------------------------------------------------------------
        boost::asio::io_context ioc_private_channel1;
        aoe::bybit::impl::test_net::private_channel::SessionRW
            session_private_channel1(ioc_private_channel1,
                                     order_manager.GetQueue(),
                                     order_manager.GetListener());

        std::string config_path = argv[1];
        OrderChannelSessionStarter order_starter(
            ioc_private_channel1, session_private_channel1, config_path);
        order_starter.Run();
        //-------------------------------------------------------------------------------
        BybitNetPositionManagerListener net_position_manager(thread_pool);
        //-------------------------------------------------------------------------------
        boost::asio::io_context ioc_private_channel2;
        aoe::bybit::impl::test_net::private_channel::SessionRW
            session_private_channel2(ioc_private_channel2,
                                     net_position_manager.GetQueue(),
                                     net_position_manager.GetListener());
        ExecutionChannelSessionStarter execution_starter(
            ioc_private_channel2, session_private_channel2, config_path);
        execution_starter.Run();
    }
    fmtlog::poll();
    return 0;
}