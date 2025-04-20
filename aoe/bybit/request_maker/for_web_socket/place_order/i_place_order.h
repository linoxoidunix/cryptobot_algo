#pragma once
#include "aos/order_types/i_order_type.h"
#include "boost/intrusive_ptr.hpp"
#include "nlohmann/json.hpp"
namespace aoe {
namespace bybit {
namespace place_order {
template <template <typename> typename MemoryPool>
class RequestMakerInterface {
  public:
    virtual nlohmann::json Make(
        boost::intrusive_ptr<aos::OrderTypeInterface<MemoryPool>> event) = 0;
    virtual ~RequestMakerInterface() = default;
};
};  // namespace place_order
};  // namespace bybit
};  // namespace aoe