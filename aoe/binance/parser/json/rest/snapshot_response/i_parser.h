#pragma once
#include "aoe/binance/order_book_event/i_order_book_event.h"
#include "boost/intrusive_ptr.hpp"
#include "simdjson.h"  // NOLINT

namespace aoe {
namespace binance {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class SnapshotEventParserInterface {
  public:
    virtual ~SnapshotEventParserInterface() = default;
    using EventPtr                          = boost::intrusive_ptr<
                                 OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>;
    virtual std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) = 0;
};
};  // namespace binance
};  // namespace aoe