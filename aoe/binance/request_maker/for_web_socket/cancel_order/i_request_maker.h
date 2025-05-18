#pragma once
#include "aos/request/i_request.h"
#include "boost/intrusive_ptr.hpp"
#include "nlohmann/json.hpp"
namespace aoe {
namespace binance {
namespace cancel_order {
template <template <typename> typename MemoryPool>
class RequestMakerInterface {
  public:
    virtual std::pair<bool, nlohmann::json> Make(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>> event) = 0;
    virtual ~RequestMakerInterface() = default;
};
};  // namespace cancel_order
};  // namespace binance
};  // namespace aoe