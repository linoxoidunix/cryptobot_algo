#include <boost/asio.hpp>

#include "aoe/bybit/execution_event/types.h"
#include "aoe/bybit/execution_watcher/execution_watcher.h"
#include "aos/aos.h"
#include "aot/common/mem_pool.h"

int main() {
    {
        using HashT = std::size_t;
        using Price = double;
        using Qty   = double;
        using namespace aos::impl;
    }
    fmtlog::poll();
    return 0;
}
