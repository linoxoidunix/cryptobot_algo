#pragma once
#include "aoe/bybit/event/funding_rate_event/i_funding_rate_event.h"
#include "boost/intrusive_ptr.hpp"
#include "simdjson.h"  // NOLINT

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class FundingRateEventParserInterface {
  public:
    virtual ~FundingRateEventParserInterface() = default;
    using EventPtr =
        boost::intrusive_ptr<FundingRateEventInterface<MemoryPool>>;
    virtual std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) = 0;
};
};  // namespace bybit
};  // namespace aoe