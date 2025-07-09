#pragma once
#include "aos/common/ref_counted.h"

namespace aos {
template <template <typename> typename MemoryPool>
class FundingRateEventInterface
    : public common::RefCounted<MemoryPool,
                                FundingRateEventInterface<MemoryPool>> {
  public:
    ~FundingRateEventInterface() override = default;
};
};  // namespace aos