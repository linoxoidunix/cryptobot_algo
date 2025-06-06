#pragma once
#include <vector>

#include "aoe/binance/enums/enums.h"
#include "aoe/binance/order_book_sync/i_order_book_sync.h"
#include "aos/common/ref_counted.h"
#include "aos/order_book_event/i_order_book_event.h"
#include "aos/order_book_level_raw/order_book_level_raw.h"
#include "aos/order_book_view/i_order_book_view.h"
#include "aos/common/exchange_id.h"

namespace aoe {
namespace binance {

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookEventInterface
    : public aos::OrderBookEventInterface<Price, Qty, MemoryPool> {
  public:
    virtual ~OrderBookEventInterface() = default;

    virtual common::ExchangeId ExchangeId() const { return exchange_id_; }
    virtual aos::TradingPair TradingPair() const { return trading_pair_; }

    virtual void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    }

    virtual void SetBids(
        std::vector<aos::OrderBookLevelRaw<Price, Qty>>&& new_bids) {
        bids_ = std::move(new_bids);
    }

    virtual void SetAsks(
        std::vector<aos::OrderBookLevelRaw<Price, Qty>>&& new_asks) {
        asks_ = std::move(new_asks);
    }

    std::vector<aos::OrderBookLevelRaw<Price, Qty>>& Bids() override {
        return bids_;
    }

    std::vector<aos::OrderBookLevelRaw<Price, Qty>>& Asks() override {
        return asks_;
    }

  protected:
    common::ExchangeId exchange_id_ = common::ExchangeId::kBinance;
    aos::TradingPair trading_pair_;
    std::vector<aos::OrderBookLevelRaw<Price, Qty>> bids_;
    std::vector<aos::OrderBookLevelRaw<Price, Qty>> asks_;
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookSnapshotEventInterface
    : public OrderBookEventInterface<Price, Qty, MemoryPool> {
  public:
    virtual ~OrderBookSnapshotEventInterface() = default;

    virtual uint64_t UpdateId() const { return update_id_; }
    virtual void SetUpdateId(uint64_t update_id) { update_id_ = update_id; }
    virtual void Accept(
        spot::OrderBookSyncInterface<Price, Qty, MemoryPool>& sync) = 0;
    virtual void Accept(
        futures::OrderBookSyncInterface<Price, Qty, MemoryPool>& sync) = 0;

  protected:
    uint64_t update_id_ = 0;
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookDiffEventInterface
    : public OrderBookEventInterface<Price, Qty, MemoryPool> {
  public:
    virtual ~OrderBookDiffEventInterface() = default;

    virtual uint64_t FirstUpdateId() const { return first_update_id_; }
    virtual uint64_t FinalUpdateId() const { return final_update_id_; }

    virtual void SetFirstUpdateId(uint64_t id) { first_update_id_ = id; }
    virtual void SetFinalUpdateId(uint64_t id) { final_update_id_ = id; }

  protected:
    uint64_t first_update_id_ = 0;
    uint64_t final_update_id_ = 0;
};
namespace spot {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookDiffEventInterface
    : public aoe::binance::OrderBookDiffEventInterface<Price, Qty, MemoryPool> {
  public:
    virtual void Accept(
        aoe::binance::spot::OrderBookSyncInterface<Price, Qty, MemoryPool>&
            sync) = 0;
};
};  // namespace spot
namespace futures {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookDiffEventInterface
    : public aoe::binance::OrderBookDiffEventInterface<Price, Qty, MemoryPool> {
  public:
    virtual ~OrderBookDiffEventInterface() = default;
    virtual uint64_t PreviousUpdateId() const { return previoues_update_id_; }
    virtual void SetPreviousUpdateId(uint64_t id) { previoues_update_id_ = id; }
    virtual void Accept(
        aoe::binance::futures::OrderBookSyncInterface<Price, Qty, MemoryPool>&
            sync) = 0;

  protected:
    uint64_t previoues_update_id_ = 0;
};
};  // namespace futures
};  // namespace binance
};  // namespace aoe