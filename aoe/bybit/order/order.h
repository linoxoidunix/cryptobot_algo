#pragma once
#include "aoe/bybit/enums/enums.h"
#include "aos/trading_pair/trading_pair.h"

namespace aoe {
namespace bybit {
namespace impl {
struct Order {
    Category category;
    Side side;
    OrderMode order_mode;
    OrderStatus order_status;
    PendingAction pending_action;
    uint64_t order_id;
    aos::TradingPair trading_pair;
    double price = 0;
    double qty   = 0;
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe