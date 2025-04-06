#pragma once
#include "aos/common/types/bbo.h"

namespace aos {
template <typename Price, typename Qty>
struct BBOQty {
    Qty qty_bid;
    Qty qty_ask;
    BBO<Price> bbo;
};
};  // namespace aos