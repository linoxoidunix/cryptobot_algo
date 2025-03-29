#include <iostream>
#include <queue>
#include <thread>
#include <vector>

#include "aos/exchange/internal/binance_exchange.h"
#include "aos/order_manager/order_manager/order_manager.h"
#include "aos/order_manager/order_submitter/order_submitter.h"
#include "aos/uid/number_pool.h"
#include "aos/uid/uid_manager.h"
#include "aos/uid/unique_id_generator.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"

int main() {
    {
        using Price = double;
        using Qty   = double;
        using Uid   = size_t;
        using MyOrderSubmitter =
            aos::impl::OrderSubmitter<Price, Qty,
                                      common::MemoryPoolNotThreadSafety, Uid>;
        using MyOrderSubmitterBuilder =
            aos::impl::OrderSubmitterBuilder<MyOrderSubmitter, Price, Qty,
                                             common::MemoryPoolNotThreadSafety,
                                             Uid>;
        using MyExchangeRegistry =
            aos::impl::ExchangeRegistry<Price, Qty,
                                        common::MemoryPoolNotThreadSafety, Uid>;
        using MyExchangeRegistryBuilder = aos::impl::ExchangeRegistryBuilder<
            MyExchangeRegistry, Price, Qty, common::MemoryPoolNotThreadSafety,
            Uid>;

        fmtlog::setLogLevel(fmtlog::INF);
        boost::asio::thread_pool thread_pool;
        // start declaration containers
        aos::impl::BinanceExchangeContainer<
            Price, Qty, common::MemoryPoolNotThreadSafety, Uid>
            binance_exchange_container(1, thread_pool);

        aos::impl::UIDManagerContainer uid_manager_container(1);

        aos::impl::OrderSubmitterContainer<
            MyOrderSubmitter, MyOrderSubmitterBuilder, MyExchangeRegistry,
            MyExchangeRegistryBuilder, Price, Qty,
            common::MemoryPoolNotThreadSafety, Uid>
            order_submit_container(1);
        // stop declaration containers
        auto uid_manager_ptr = uid_manager_container.Build();

        auto [order_submitter_ptr, exchange_registry_ptr] =
            order_submit_container.Build();
        aos::impl::OrderManager<double, double, size_t> order_manager(
            order_submitter_ptr, uid_manager_ptr);

        auto binance_exchange_ptr = binance_exchange_container.Build();

        binance_exchange_ptr->Subscribe(&order_manager);
        exchange_registry_ptr->RegisterExchange(aos::OrderRouting::Internal,
                                                common::ExchangeId::kBinance,
                                                binance_exchange_ptr);

        order_manager.PlaceOrder(common::ExchangeId::kBinance, {2, 1}, 100, 10,
                                 common::Side::kAsk, aos::TimeInForce::GTC,
                                 aos::OrderRouting::Internal);
        order_manager.CancelOrder(common::ExchangeId::kBinance,
                                  aos::OrderRouting::Internal, 0);
    }
    fmtlog::poll();
    return 0;
}