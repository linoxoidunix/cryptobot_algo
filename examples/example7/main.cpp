#include <map>
#include <unordered_map>

#include "aos/exchange/internal/binance_exchange.h"
#include "aos/order_manager/order_manager/order_manager.h"
#include "aos/order_manager/order_submitter/order_submitter.h"
#include "aos/uid/number_pool.h"
#include "aos/uid/uid_manager.h"
#include "aos/uid/unique_id_generator.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"

// Тестовая структура (например, для типа Price и Qty)
using Price = double;
using Qty   = double;
using Uid   = size_t;

int main() {
    {
        boost::asio::thread_pool thread_pool;
        aos::impl::BinanceExchangeContainer<double, double, size_t>
            binance_exchange_container(1, thread_pool);
        auto binance_exchange_ptr = binance_exchange_container.Build();

        // Создаем пул памяти
        common::MemoryPoolNotThreadSafety<aos::impl::ExchangeRegistry<
            Price, Qty, common::MemoryPoolNotThreadSafety, Uid>>
            pool(1);

        // Строим экземпляр FakeExchangeRegistry через
        aos::impl::ExchangeRegistryBuilder<
            Price, Qty, common::MemoryPoolNotThreadSafety, Uid>
            builder(pool);
        auto registry = builder.Build();

        // Регистрируем обмен
        registry->RegisterExchange(aos::OrderRouting::Internal,
                                   common::ExchangeId::kBinance,
                                   binance_exchange_ptr);
    }
    fmtlog::poll();

    return 0;
}
