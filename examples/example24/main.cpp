#include "aoe/bybit/api/external/rest/exchange_api.h"
#include "aos/common/mem_pool.h"

int main() {
    {
        aoe::bybit::impl::external::rest::SingleOrderAPI<
            common::MemoryPoolThreadSafety>
            api;
    }
    return 0;
}