#include "aoe/aoe.h"
#include "aos/aos.h"

int main() {
    {
        aoe::bybit::impl::internal::SingleOrderAPI<
            common::MemoryPoolThreadSafety>
            bybit_api;
        bybit_api.PlaceOrder();
        bybit_api.AmendOrder();
        bybit_api.CancelOrder();
        bybit_api.CancelAllOrder();
    }
    return 0;
}