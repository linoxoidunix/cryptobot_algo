#pragma once
#include "aoe/binance/order_book_sync/i_order_book_sync.h"
#include "aos/order_book/order_book.h"
#include "aot/Logger.h"
#include "boost/asio.hpp"

namespace aoe {
namespace binance {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename HashMap>
class OrderBookSync : public OrderBookSyncInterface<Price, Qty, MemoryPool> {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    aos::OrderBookEventListener<Price, Qty, MemoryPool, HashMap>& order_book_;
    uint64_t current_diff_update_id_;
    uint64_t current_snapshot_update_id_;
    bool received_snapshot_success_ = false;

  public:
    OrderBookSync(boost::asio::thread_pool& thread_pool,
                  aos::OrderBookEventListener<Price, Qty, MemoryPool, HashMap>&
                      order_book)
        : strand_(boost::asio::make_strand(thread_pool)),
          order_book_(order_book) {}
    ~OrderBookSync() override = default;
    void OnEvent(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) override {
        ptr->Accept(*this);
    };
    void AcceptSnapshot(boost::intrusive_ptr<
                        OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
                            ptr) override {
        boost::asio::co_spawn(strand_, ProcessSnapshot(ptr),
                              boost::asio::detached);
    };
    void AcceptDiff(boost::intrusive_ptr<
                    OrderBookDiffEventInterface<Price, Qty, MemoryPool>>
                        ptr) override {
        boost::asio::co_spawn(strand_, ProcessDiff(ptr), boost::asio::detached);
    };

  private:
    boost::asio::awaitable<void> ProcessSnapshot(
        boost::intrusive_ptr<
            OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
            ptr) {
        received_snapshot_success_  = true;
        current_snapshot_update_id_ = ptr->UpdateId();
        order_book_.OnEvent(ptr);
        co_return;
    }
    boost::asio::awaitable<void> ProcessDiff(
        boost::intrusive_ptr<
            OrderBookDiffEventInterface<Price, Qty, MemoryPool>>
            ptr) {
        // если всё хорошо, отправляем diff в книгу ордеров
        // если всё плохо:
        // 1)буферизуем дифф
        // 2)делаем снапшот
        // 3)вызываем AcceptSnapshot

        // if (!received_snapshot_success_) co_return;
        // auto new_update_id = ptr->UpdateId();
        // if (current_diff_update_id_ + 1 != new_update_id) {
        //     order_book_.Clear();
        //     received_snapshot_success_  = false;
        //     current_diff_update_id_     = 0;
        //     current_snapshot_update_id_ = 0;
        //     co_return;
        // }
        order_book_.OnEvent(ptr);
        // current_diff_update_id_ = new_update_id;
        co_return;
    }
};
};  // namespace impl
};  // namespace binance
};  // namespace aoe