#include "aoe/aoe.h"
#include "aos/aos.h"

int main() {
    {
        aoe::bybit::impl::internal::SingleOrderAPI<
            common::MemoryPoolThreadSafety>
            bybit_api;
    }
    return 0;
}