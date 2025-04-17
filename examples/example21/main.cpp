#include "aoe/bybit/order_manager/order_manager.h"
#include "aos/aos.h"
#include "aos/multi_order_manager/multi_order_manager.h"
#include "aot/common/mem_pool.h"

struct DummyPosition {};
using PositionT = aos::impl::NetPositionDefault<double, double>;

int main() {
    {
        aos::impl::MultiOrderManagerDefault<common::MemoryPoolThreadSafety>
            multi_order_manager;
        auto ptr = std::make_unique<aoe::bybit::impl::OrderManagerDefault<
            common::MemoryPoolThreadSafety>>();
        multi_order_manager.Register(common::ExchangeId::kBybit,
                                     std::move(ptr));
    }
    return 0;
}