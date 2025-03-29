#pragma once

#include "aos/order_manager/order_manager_types.h"

namespace aos {
class IOrderStatusProvider {
  public:
    virtual std::pair<bool, OrderStatus> GetStatus(size_t order_id) = 0;
    virtual ~IOrderStatusProvider()                                 = default;
};
};  // namespace aos