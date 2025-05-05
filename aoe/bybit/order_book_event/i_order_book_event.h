#pragma once
#include <vector>

#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/order_book_sync/i_order_book_sync.h"
#include "aos/common/ref_counted.h"
#include "aos/order_book_event/i_order_book_event.h"
#include "aos/order_book_level_raw/order_book_level_raw.h"
#include "aos/order_book_view/i_order_book_view.h"
#include "aot/common/types.h"

namespace aoe {
namespace bybit {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookEventInterface
    : public aos::OrderBookTwoSideViewInterface<Price, Qty>,
      public aos::OrderBookEventInterface<MemoryPool> {
  public:
    virtual ~OrderBookEventInterface() = default;
    virtual uint64_t UpdateId() const { return update_id_; };

    virtual common::ExchangeId ExchangeId() const { return exchange_id; };
    virtual common::TradingPair TradingPair() const { return trading_pair_; };

    virtual void SetTradingPair(common::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    };
    virtual void SetUpdateId(uint64_t update_id) { update_id_ = update_id; };
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

    virtual void Accept(
        OrderBookSyncInterface<Price, Qty, MemoryPool>& sync) = 0;

  protected:
    aoe::bybit::OrderStatus order_status_ = aoe::bybit::OrderStatus::kInvalid;
    uint64_t update_id_                   = 0;
    common::ExchangeId exchange_id        = common::ExchangeId::kBybit;
    common::TradingPair trading_pair_;
    std::vector<aos::OrderBookLevelRaw<Price, Qty>> bids_;
    std::vector<aos::OrderBookLevelRaw<Price, Qty>> asks_;
};
};  // namespace bybit
};  // namespace aoe