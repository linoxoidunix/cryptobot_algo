#pragma once
#include "aoe/bybit/order_book_sync/i_order_book_sync.h"
#include "aos/logger/mylog.h"
#include "aos/order_book/i_order_book.h"
#include "boost/asio.hpp"

namespace aoe {
namespace bybit {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename HashMap>
class OrderBookSync : public OrderBookSyncInterface<Price, Qty, MemoryPool> {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    aos::OrderBookEventListenerInterface<Price, Qty, MemoryPool>& order_book_;
    uint64_t current_diff_update_id_;
    uint64_t current_snapshot_update_id_;
    bool received_snapshot_success_ = false;

  public:
    OrderBookSync(boost::asio::thread_pool& thread_pool,
                  aos::OrderBookEventListenerInterface<Price, Qty, MemoryPool>&
                      order_book)
        : strand_(boost::asio::make_strand(thread_pool)),
          order_book_(order_book) {}
    ~OrderBookSync() override = default;
    void OnEvent(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) override {
        ptr->Accept(*this);
    };
    void AcceptSnapshot(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) override {
        boost::asio::co_spawn(strand_, ProcessSnapshot(ptr),
                              boost::asio::detached);
    };
    void AcceptDiff(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) override {
        boost::asio::co_spawn(strand_, ProcessDiff(ptr), boost::asio::detached);
    };

  private:
    boost::asio::awaitable<void> ProcessSnapshot(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) {
        received_snapshot_success_ = true;
        auto new_snapshot_id       = ptr->UpdateId();
        if (current_snapshot_update_id_ == new_snapshot_id) {
            // https://bybit-exchange.github.io/docs/v5/websocket/public/orderbook
            // For level 1 of linear, inverse Perps and Futures, the snapshot
            // data will be pushed again when there is no change in 3 seconds,
            // and the "u" will be the same as that in the previous message
            co_return;
        }

        current_snapshot_update_id_ = ptr->UpdateId();
        if (current_snapshot_update_id_ == 1) {
            // https://bybit-exchange.github.io/docs/v5/websocket/public/orderbook
            // Occasionally, you'll receive "u"=1, which is a snapshot data due
            // to the restart of the service. So please overwrite your local
            // orderbook
            order_book_.Clear();
        }
        current_diff_update_id_ = current_snapshot_update_id_;
        order_book_.OnEvent(ptr);
        co_return;
    }
    boost::asio::awaitable<void> ProcessDiff(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) {
        if (!received_snapshot_success_) co_return;
        auto new_update_id = ptr->UpdateId();
        if (current_diff_update_id_ + 1 != new_update_id) {
            order_book_.Clear();
            received_snapshot_success_  = false;
            current_diff_update_id_     = 0;
            current_snapshot_update_id_ = 0;
            co_return;
        }
        order_book_.OnEvent(ptr);
        current_diff_update_id_ = new_update_id;
        co_return;
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe