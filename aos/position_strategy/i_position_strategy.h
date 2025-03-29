#pragma once

#include "aos/common/ref_counted.h"

namespace aos {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class IPositionStrategy
    : public common::RefCounted<MemoryPool,
                                IPositionStrategy<Price, Qty, MemoryPool>> {
  public:
    virtual ~IPositionStrategy()       = default;
    virtual void Add(Price& avg_price, Qty& net_qty, Price price,
                     Qty qty) const    = 0;
    virtual void Remove(Price& avg_price, Qty& net_qty, Price price,
                        Qty qty) const = 0;
};
};  // namespace aos