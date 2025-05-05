#pragma once
#include "aoe/bybit/order_book_event/i_order_book_event.h"
#include "boost/intrusive_ptr.hpp"
#include "simdjson.h"

namespace aoe {
namespace bybit {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookEventParserInterface {
  public:
    virtual ~OrderBookEventParserInterface() = default;
    using EventPtr =
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>;
    virtual std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) = 0;
};
};  // namespace bybit
};  // namespace aoe