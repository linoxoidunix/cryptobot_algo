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

using BybitSpotBuyLimit =
    aoe::bybit::place_order::impl::SpotBuyLimit<common::MemoryPoolThreadSafety>;
auto Allocate(common::MemoryPoolThreadSafety<BybitSpotBuyLimit>& pool,
              aos::impl::TradingPairPrinter& printer) {
    auto ptr = pool.Allocate(printer);
    ptr->SetMemoryPool(&pool);
    return ptr;
}

int main(int argc, char** argv) {
    {
        aos::impl::TradingPairPrinter trading_pair_printer;

        BybitSpotBuyLimit order(trading_pair_printer);
        common::MemoryPoolThreadSafety<BybitSpotBuyLimit>
            memory_pool_bybit_spot_buy_limit{100};
        //-------------------------------------------------------------------------
        auto ptr_order =
            Allocate(memory_pool_bybit_spot_buy_limit, trading_pair_printer);
        //-------------------------------------------------------------------------
        aoe::impl::WebSocketSessionDummy wss;
        aoe::impl::WebSocketSessionProvider<common::MemoryPoolThreadSafety>
            wss_provider(wss);

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
        multi_order_manager.Register(common::ExchangeId::kBybit,
                                     std::move(ptr));
        //-------------------------------------------------------------------------
        multi_order_manager.PlaceOrder(common::ExchangeId::kBybit, ptr_order);
    }
    fmtlog::poll();
    return 0;
}