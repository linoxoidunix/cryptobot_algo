#pragma once

#include "aoe/bybit/event/funding_rate_event/i_funding_rate_event.h"

namespace aoe::bybit::impl {
template <template <typename> typename MemoryPool>
class FundingRateEvent : public FundingRateEventInterface<MemoryPool> {
  public:
    ~FundingRateEvent() override = default;

};  // namespace bybit
};  // namespace aoe