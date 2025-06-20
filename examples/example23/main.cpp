#include "aoe/bybit/api/internal/exchange_api.h"
#include "aos/common/mem_pool.h"

int main() {
    {
        aoe::bybit::impl::internal::SingleOrderAPI<
            common::MemoryPoolThreadSafety>
            api;
    }
    return 0;
}