#pragma once
#include "aoe/binance/order_event/i_order_event.h"
#include "boost/intrusive_ptr.hpp"
#include "simdjson.h"  // NOLINT

namespace aoe {
namespace binance {
template <template <typename> typename MemoryPool>
class OrderEventParserInterface {
  public:
    virtual ~OrderEventParserInterface() = default;
    using EventPtr = boost::intrusive_ptr<OrderEventInterface<MemoryPool>>;
    virtual std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) = 0;
};
};  // namespace binance
};  // namespace aoe