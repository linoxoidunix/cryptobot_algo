#include "aoe/aoe.h"
#include "aos/aos.h"

int main() {
    {
        aoe::bybit::impl::external::rest::SingleOrderAPI<
            common::MemoryPoolThreadSafety>
            api;
    }
    return 0;
}