#pragma once
#include <cstdint>

#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/order/order.h"

namespace aoe {
namespace bybit {
class OrderMutatorInterface {
  public:
    virtual ~OrderMutatorInterface() = default;

    virtual void UpdateStatus(uint64_t order_id,
                              aoe::bybit::OrderStatus new_order_status,
                              aoe::bybit::PendingAction new_pending_action) = 0;

    virtual void Remove(uint64_t order_id)                                  = 0;

    virtual void UpdateState(uint64_t order_id, double new_price,
                             double new_qty)                                = 0;
    virtual void Add(impl::Order&& order)                                   = 0;
};

};  // namespace bybit
};  // namespace aoe