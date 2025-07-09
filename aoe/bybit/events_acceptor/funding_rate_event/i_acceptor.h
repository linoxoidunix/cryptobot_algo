#pragma once
#include "boost/intrusive_ptr.hpp"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class FundingRateEventInterface;
};
};  // namespace aoe
namespace aoe {
namespace bybit {

template <template <typename> typename MemoryPool>
class FundingRateEventAcceptorInterface {
  public:
    virtual ~FundingRateEventAcceptorInterface() = default;
    virtual void OnEvent(
        boost::intrusive_ptr<FundingRateEventInterface<MemoryPool>> ptr) = 0;
};
};  // namespace bybit
};  // namespace aoe
