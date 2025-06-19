#pragma once
#include <vector>

#include "aoe/binance/enums/enums.h"
#include "aoe/binance/order/order.h"
#include "aoe/binance/order_mutator/i_order_mutator.h"
namespace aoe {
namespace binance {
class OrderStorageInterface : public OrderMutatorInterface {
  public:
    ~OrderStorageInterface() override = default;
    virtual std::vector<const impl::Order*> Get(
        aoe::binance::Category category)                                 = 0;
    virtual std::vector<const impl::Order*> Get(aoe::binance::Category category,
                                                aoe::binance::Side side) = 0;
    virtual std::vector<const impl::Order*> Get(
        aoe::binance::OrderStatus status) = 0;
    virtual std::vector<const impl::Order*> Get(
        aoe::binance::Category category,
        aoe::binance::OrderMode order_mode)                            = 0;
    virtual std::pair<bool, const impl::Order*> Get(uint64_t order_id) = 0;
};
};  // namespace binance
};  // namespace aoe