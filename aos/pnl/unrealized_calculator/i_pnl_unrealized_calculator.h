#pragma once
#include "aos/common/exchange_id.h"
#include "aos/common/ref_counted.h"

namespace aos {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class IUnRealizedPnlCalculator
    : public common::RefCounted<
          MemoryPool, IUnRealizedPnlCalculator<Price, Qty, MemoryPool>> {
  public:
    using UnRealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());

    virtual UnRealizedPnl Calculate(Price avg_price, Qty net_qty, Price bid,
                                    Price ask) = 0;
    ~IUnRealizedPnlCalculator() override       = default;
};

template <typename Price, typename Qty>
class UnRealizedPnlCalculatorInterface {
  public:
    using UnRealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());

    virtual UnRealizedPnl Calculate(Price avg_price, Qty net_qty, Price bid,
                                    Price ask)  = 0;

    virtual ~UnRealizedPnlCalculatorInterface() = default;
};
};  // namespace aos