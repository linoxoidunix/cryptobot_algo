#pragma once
#include "aos/common/ref_counted.h"
#include "aot/common/types.h"

namespace aos {
template <typename Price, typename Qty, template <typename> class MemoryPool>
, typename Uid > class IPositionStorage
    : public common::RefCounted<MemoryPool,
                                IPositionStorage<Price, Qty, MemoryPool>> {
  public:
    virtual ~IPositionStorage()                = default;
    virtual void AddPosition(const common::TradingPair& pair, Price price,
                             Qty qty, Uid uid) = 0;
    virtual void UpdatePosition(const common::TradingPair& pair, Qty qty) = 0;
    virtual Qty GetPosition(const common::TradingPair& pair) const        = 0;
};
};  // namespace aos