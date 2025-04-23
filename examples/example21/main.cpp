#include "aoe/aoe.h"
#include "aoe/bybit/order_manager/order_manager.h"
#include "aoe/bybit/order_storage/order_storage.h"
#include "aos/aos.h"
#include "aos/multi_order_manager/multi_order_manager.h"
#include "aot/common/mem_pool.h"

struct DummyPosition {};
using PositionT = aos::impl::NetPositionDefault<double, double>;

int main() {
    {
        aoe::impl::WebSocketSession<common::MemoryPoolThreadSafety> wss;
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
    }
    return 0;
}