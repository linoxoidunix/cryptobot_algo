#pragma once
#include "aoe/binance/order_book_event/i_order_book_event.h"
#include "boost/intrusive_ptr.hpp"
#include "simdjson.h"  // NOLINT

namespace aoe {
namespace binance {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename DiffType>
class DiffEventParserInterface {
  public:
    virtual ~DiffEventParserInterface() = default;
    using EventPtr                      = boost::intrusive_ptr<DiffType>;
    virtual std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) = 0;
};
// namespace spot {
// template <typename Price, typename Qty, template <typename> typename
// MemoryPool> class DiffEventParserInterface {
//   public:
//     virtual ~DiffEventParserInterface() = default;
//     using EventPtr =
//         boost::intrusive_ptr<aoe::binance::spot::OrderBookDiffEventInterface<
//             Price, Qty, MemoryPool>>;
//     virtual std::pair<bool, EventPtr> ParseAndCreate(
//         simdjson::ondemand::document& doc) = 0;
// };
// };  // namespace spot
// namespace futures {
// template <typename Price, typename Qty, template <typename> typename
// MemoryPool> class DiffEventParserInterface {
//   public:
//     virtual ~DiffEventParserInterface() = default;
//     using EventPtr =
//         boost::intrusive_ptr<aoe::binance::futures::OrderBookDiffEventInterface<
//             Price, Qty, MemoryPool>>;
//     virtual std::pair<bool, EventPtr> ParseAndCreate(
//         simdjson::ondemand::document& doc) = 0;
// };
// };  // namespace futures
};  // namespace binance
};  // namespace aoe