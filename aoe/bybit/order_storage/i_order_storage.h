#pragma once
#include <vector>

#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/order/order.h"
#include "aoe/bybit/order_mutator/i_order_mutator.h"
namespace aoe {
namespace bybit {
class OrderStorageInterface : public OrderMutatorInterface {
  public:
    ~OrderStorageInterface() override = default;
    virtual std::vector<const impl::Order*> Get(
        aoe::bybit::Category category)                                 = 0;
    virtual std::vector<const impl::Order*> Get(aoe::bybit::Category category,
                                                aoe::bybit::Side side) = 0;
    virtual std::vector<const impl::Order*> Get(
        aoe::bybit::OrderStatus status) = 0;
    virtual std::vector<const impl::Order*> Get(
        aoe::bybit::Category category, aoe::bybit::OrderMode order_mode) = 0;
    virtual std::pair<bool, const impl::Order*> Get(uint64_t order_id)   = 0;
};
};  // namespace bybit
};  // namespace aoe