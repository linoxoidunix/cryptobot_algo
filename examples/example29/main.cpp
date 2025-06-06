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
#include "aos/trading_pair/trading_pair.h"
#include "aos/common/mem_pool.h"
#include "aos/logger/logger.h"


using BybitSpotBuyLimit =
    aoe::bybit::place_order::impl::SpotBuyLimit<common::MemoryPoolThreadSafety>;
auto AllocatePlaceOrder(
    common::MemoryPoolThreadSafety<BybitSpotBuyLimit>& pool) {
    auto ptr = pool.Allocate();
    ptr->SetMemoryPool(&pool);
    return ptr;
}

using BybitSpotCancel =
    aoe::bybit::cancel_order::impl::Spot<common::MemoryPoolThreadSafety>;

auto AllocateCancelOrder(
    common::MemoryPoolThreadSafety<BybitSpotCancel>& pool) {
    auto ptr = pool.Allocate();
    ptr->SetMemoryPool(&pool);
    return ptr;
}

int main(int argc, char** argv) {
    {
        boost::asio::thread_pool thread_pool;
        LogPolling log_polling(thread_pool, std::chrono::microseconds(1));
        //-------------------------------------------------------------------------

        common::MemoryPoolThreadSafety<BybitSpotBuyLimit>
            memory_pool_bybit_spot_buy_limit{100};
        common::MemoryPoolThreadSafety<BybitSpotCancel>
            memory_pool_bybit_spot_cancel{100};
        //-------------------------------------------------------------------------
        std::string config_path = argv[1];
        aoe::bybit::impl::CredentialsLoader bybit_credentials(config_path);
        //-------------------------------------------------------------------------
        boost::asio::io_context ioc_trade;
        aoe::bybit::impl::test_net::trade_channel::SessionW ws_order(ioc_trade);
        //-------------------------------------------------------------------------
        boost::asio::steady_timer timer_order(ioc_trade);
        aoe::bybit::impl::private_channel::PingManager<std::chrono::seconds>
            ping_manager_order(timer_order, ws_order, std::chrono::seconds(20));
        aoe::bybit::impl::test_net::PrivateSessionSetup
            private_session_setuper_order(ws_order, bybit_credentials,
                                          ping_manager_order);
        bool apikey_readed = private_session_setuper_order.Setup();
        if (!apikey_readed) return -1;
        //-------------------------------------------------------------------------
        auto ptr_place_order =
            AllocatePlaceOrder(memory_pool_bybit_spot_buy_limit);
        ptr_place_order->SetTradingPair(aos::TradingPair::kBTCUSDT);
        ptr_place_order->SetOrderSide(aoe::bybit::Side::kBuy);
        ptr_place_order->SetOrderMode(aoe::bybit::OrderMode::kLimit);
        ptr_place_order->SetPrice(50000);
        ptr_place_order->SetQty(0.0001);
        ptr_place_order->SetTimeInForce(aoe::bybit::TimeInForce::kFOK);
        //-------------------------------------------------------------------------
        auto ptr_cancel_order =
            AllocateCancelOrder(memory_pool_bybit_spot_cancel);
        ptr_cancel_order->SetTradingPair(aos::TradingPair::kBTCUSDT);
        ptr_cancel_order->SetOrderId(0);
        //-------------------------------------------------------------------------
        aoe::impl::WebSocketSessionProvider<common::MemoryPoolThreadSafety>
            wss_provider(ws_order);

        aos::impl::NumberPoolDefault<uint64_t> number_pool;
        aos::impl::UIDGeneratorDefault<uint64_t> uid_generator;
        aos::impl::UIDManagerDefault<uint64_t> uid_manager(uid_generator,
                                                           number_pool);

        aoe::bybit::impl::external::ws::SingleOrderAPIDefault<
            common::MemoryPoolThreadSafety>
            api(wss_provider);
        aos::impl::MultiOrderManagerDefault<common::MemoryPoolThreadSafety>
            multi_order_manager;
        aoe::bybit::impl::OrderStorage order_storage;
        auto ptr = std::make_unique<
            aoe::bybit::impl::OrderManager<common::MemoryPoolThreadSafety>>(
            order_storage, api, uid_manager);
        multi_order_manager.Register(common::ExchangeId::kBybit,
                                     std::move(ptr));
        //-------------------------------------------------------------------------
        // multi_order_manager.CancelOrder(common::ExchangeId::kBybit,
        //                                 ptr_cancel_order);
        //-------------------------------------------------------------------------
        multi_order_manager.PlaceOrderManualId(common::ExchangeId::kBybit,
                                               ptr_place_order, 3);
        //-------------------------------------------------------------------------
        std::thread thread_ioc_trade([&ioc_trade]() { ioc_trade.run(); });
        //-------------------------------------------------------------------------
        thread_ioc_trade.join();
    }
    fmtlog::poll();
    return 0;
}