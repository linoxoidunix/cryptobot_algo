#pragma once
#include "aoe/bybit/order_event/i_order_event.h"
#include "boost/intrusive_ptr.hpp"
#include "simdjson.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class OrderEventParserInterface {
  public:
    using EventPtr = boost::intrusive_ptr<OrderEventInterface<MemoryPool>>;
    virtual std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) = 0;
};
};  // namespace bybit
};  // namespace aoe