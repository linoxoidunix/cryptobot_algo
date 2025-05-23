#pragma once
#include <queue>

#include "aoe/binance/order_book_sync/i_order_book_sync.h"
#include "aoe/binance/parser/json/rest/snapshot_response/parser.h"
#include "aoe/binance/response_queue_listener/json/rest/snapshot/listener.h"
#include "aoe/binance/rest_request_sender/rest_request_sender.h"
#include "aoe/binance/session/rest/session.h"
#include "aos/order_book/order_book.h"
#include "aot/Logger.h"
#include "boost/asio.hpp"

namespace aoe {
namespace binance {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename HashMap>
class OrderBookSyncBase
    : public OrderBookSyncInterface<Price, Qty, MemoryPool> {
  protected:
    boost::asio::thread_pool& thread_pool_;
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    aos::OrderBookEventListener<Price, Qty, MemoryPool, HashMap>& order_book_;
    uint64_t last_diff_update_id_       = 0;
    uint64_t last_snapshot_update_id_   = 0;

    bool need_make_snapshot_            = true;
    bool need_process_current_snapshot_ = false;
    bool need_process_current_diff_     = false;
    bool diff_packet_lost_              = true;
    bool snapshot_request_in_flight_    = false;

    boost::intrusive_ptr<
        OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
        last_snapshot_;
    std::queue<boost::intrusive_ptr<
        OrderBookDiffEventInterface<Price, Qty, MemoryPool>>>
        diffs_;

  public:
    OrderBookSyncBase(boost::asio::thread_pool& thread_pool,
                      aos::OrderBookEventListener<Price, Qty, MemoryPool,
                                                  HashMap>& order_book)
        : thread_pool_(thread_pool),
          strand_(boost::asio::make_strand(thread_pool)),
          order_book_(order_book) {}

    void OnEvent(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) override {
        boost::asio::co_spawn(
            strand_,
            [ptr, this]() -> boost::asio::awaitable<void> {
                ptr->Accept(*this);
                co_return;
            },
            boost::asio::detached);
    }

    void AcceptSnapshot(boost::intrusive_ptr<
                        OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
                            ptr) override {
        boost::asio::co_spawn(strand_, ProcessSnapshot(ptr),
                              boost::asio::detached);
    }

    void AcceptDiff(boost::intrusive_ptr<
                    OrderBookDiffEventInterface<Price, Qty, MemoryPool>>
                        ptr) override {
        boost::asio::co_spawn(strand_, ProcessDiff(ptr), boost::asio::detached);
    }

  protected:
    virtual bool CheckPacketLost(
        boost::intrusive_ptr<
            OrderBookDiffEventInterface<Price, Qty, MemoryPool>>
            diff)                                                 = 0;
    virtual void SetLastDiffUpdateId(uint64_t new_diff_update_id) = 0;
    virtual boost::asio::awaitable<void> RequestNewSnapshot(
        aos::TradingPair) = 0;
    boost::asio::awaitable<void> ProcessSnapshot(
        boost::intrusive_ptr<
            OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
            ptr) {
        last_snapshot_update_id_    = ptr->UpdateId();
        last_snapshot_              = ptr;
        snapshot_request_in_flight_ = false;
        logd("Snapshot received, clearing in-flight flag");
        // skip earlier diffs as soon as possible
        while (!diffs_.empty()) {
            auto ptr         = diffs_.front();
            const auto& diff = *ptr;
            if ((diff.FirstUpdateId() <= last_snapshot_update_id_ + 1) &&
                (diff.FinalUpdateId() >= last_snapshot_update_id_ + 1)) {
                break;
            }
            logd(
                "skip diff: FirstUpdateId={}, FinalUpdateId={} "
                "last_snapshot_update_id_={}",
                diff.FirstUpdateId(), diff.FinalUpdateId(),
                last_snapshot_update_id_);
            SetLastDiffUpdateId(diff.FinalUpdateId());
            diffs_.pop();
        }
        co_return;
    }
    boost::asio::awaitable<void> ProcessDiff(
        boost::intrusive_ptr<
            OrderBookDiffEventInterface<Price, Qty, MemoryPool>>
            ptr) {
        diffs_.push(ptr);
        const auto& diff = *ptr;
        logd("Received diff: FirstUpdateId={}, FinalUpdateId={} {}",
             diff.FirstUpdateId(), diff.FinalUpdateId(), diff.TradingPair());

        auto first_diff   = diffs_.front();
        diff_packet_lost_ = CheckPacketLost(first_diff);
        logd("Packet lost check result: {}", diff_packet_lost_);

        SetLastDiffUpdateId(first_diff->FinalUpdateId());
        logd("Set last diff update id to {}", first_diff->FinalUpdateId());

        if (need_make_snapshot_) {
            const bool snapshot_and_diff_now_sync =
                (first_diff->FirstUpdateId() <= last_snapshot_update_id_ + 1) &&
                (first_diff->FinalUpdateId() >= last_snapshot_update_id_ + 1);
            logd(
                "Checking snapshot-diff sync: last_snapshot_update_id_={}, "
                "snapshot_and_diff_synced={}",
                last_snapshot_update_id_, snapshot_and_diff_now_sync);

            if (snapshot_and_diff_now_sync) {
                need_make_snapshot_            = false;
                need_process_current_snapshot_ = true;
                logi(
                    "[SYNC ACHIEVED] Pair={} Snapshot and diff are "
                    "synchronized",
                    first_diff->TradingPair());
            }
        }

        const bool need_snapshot = need_make_snapshot_ || diff_packet_lost_;
        logd("Need snapshot? {}", need_snapshot);

        if (need_snapshot && !snapshot_request_in_flight_) {
            if (first_diff->FinalUpdateId() <= last_snapshot_update_id_) {
                logi(
                    "[OUTDATED DIFF] FinalUpdateId={} <= "
                    "last_snapshot_update_id_={} — skipping",
                    first_diff->FinalUpdateId(), last_snapshot_update_id_);
                co_return;
            }

            logi("Requesting new snapshot for pair {}", ptr->TradingPair());
            need_make_snapshot_         = true;
            snapshot_request_in_flight_ = true;
            boost::asio::co_spawn(thread_pool_,
                                  RequestNewSnapshot(ptr->TradingPair()),
                                  boost::asio::detached);
            co_return;
        }
        if (need_snapshot && snapshot_request_in_flight_) {
            logi(
                "Snapshot already requested, skipping duplicate request for {}",
                ptr->TradingPair());
            co_return;
        }

        if (need_process_current_snapshot_) {
            need_process_current_snapshot_ = false;
            need_process_current_diff_     = true;
            logi("Applying last snapshot to order book for {}",
                 ptr->TradingPair());
            order_book_.OnEvent(last_snapshot_);
        }

        if (!diff_packet_lost_ && need_process_current_diff_) {
            logi("Applying diff to order book for {}", ptr->TradingPair());
            auto ptr = diffs_.front();
            order_book_.OnEvent(ptr);
            diffs_.pop();
            while (!diffs_.empty()) {
                auto first_diff   = diffs_.front();
                diff_packet_lost_ = CheckPacketLost(first_diff);
                logd("Packet lost check result: {}", diff_packet_lost_);
                if (diff_packet_lost_) {
                    // need skip next;
                }
                SetLastDiffUpdateId(first_diff->FinalUpdateId());
                logd("Set last diff update id to {}",
                     first_diff->FinalUpdateId());
                diffs_.pop();
                order_book_.OnEvent(first_diff);
            }
        }

        co_return;
    }
};

namespace main_net {
namespace spot {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename HashMap>
class OrderBookSync
    : public OrderBookSyncBase<Price, Qty, MemoryPool, HashMap> {
    using Base = OrderBookSyncBase<Price, Qty, MemoryPool, HashMap>;
    aoe::binance::impl::SnapshotEventParser<Price, Qty, MemoryPool>
        snapshot_parser_{10};
    moodycamel::ConcurrentQueue<std::vector<char>> response_queue_;
    aoe::binance::impl::snapshot::Listener<Price, Qty, MemoryPool> listener_;
    boost::asio::thread_pool& thread_pool_;

  public:
    OrderBookSync(boost::asio::thread_pool& thread_pool,
                  aos::OrderBookEventListener<Price, Qty, MemoryPool, HashMap>&
                      order_book)
        : Base(thread_pool, order_book),
          listener_(thread_pool, response_queue_, snapshot_parser_, *this),
          thread_pool_(thread_pool) {}
    ~OrderBookSync() override = default;

  protected:
    bool CheckPacketLost(boost::intrusive_ptr<
                         OrderBookDiffEventInterface<Price, Qty, MemoryPool>>
                             diff) override {
        // реализация для main_net spot
        logd("diff_first_id:{} last_diff_update_id:{}", diff->FirstUpdateId(),
             OrderBookSyncBase<Price, Qty, MemoryPool,
                               HashMap>::last_diff_update_id_);

        return (diff->FirstUpdateId() !=
                OrderBookSyncBase<Price, Qty, MemoryPool,
                                  HashMap>::last_diff_update_id_ +
                    1);
    }
    void SetLastDiffUpdateId(uint64_t new_diff_update_id) override {
        OrderBookSyncBase<Price, Qty, MemoryPool,
                          HashMap>::last_diff_update_id_ = new_diff_update_id;
    }
    boost::asio::awaitable<void> RequestNewSnapshot(
        aos::TradingPair pair) override {
        boost::asio::io_context ioc;

        aoe::binance::impl::main_net::spot::RestSessionRW session(
            ioc, response_queue_, listener_);
        aoe::binance::snapshot::impl::SnapshotRequestSender<MemoryPool> sender(
            session);
        aoe::binance::snapshot::main_net::spot::impl::Request<MemoryPool>
            request;
        request.SetTradingPair(pair);
        // default depth = 5000;
        sender.Send(request);
        auto thread_ = std::jthread([&ioc]() { ioc.run(); });
        co_return;
    }
};
};  // namespace spot
};  // namespace main_net

namespace main_net {
namespace futures {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename HashMap>
class OrderBookSync
    : public OrderBookSyncBase<Price, Qty, MemoryPool, HashMap> {
    using Base = OrderBookSyncBase<Price, Qty, MemoryPool, HashMap>;
    aoe::binance::impl::SnapshotEventParser<Price, Qty, MemoryPool>
        snapshot_parser_{10};
    moodycamel::ConcurrentQueue<std::vector<char>> response_queue_;
    aoe::binance::impl::snapshot::Listener<Price, Qty, MemoryPool> listener_;

    boost::asio::thread_pool& thread_pool_;

  public:
    OrderBookSync(boost::asio::thread_pool& thread_pool,
                  aos::OrderBookEventListener<Price, Qty, MemoryPool, HashMap>&
                      order_book)
        : Base(thread_pool, order_book),
          listener_(thread_pool, response_queue_, snapshot_parser_, *this),
          thread_pool_(thread_pool) {}
    ~OrderBookSync() override = default;

  protected:
    bool CheckPacketLost(
        boost::intrusive_ptr<
            OrderBookDiffEventInterface<Price, Qty, MemoryPool>>) override {
        // реализация для main_net futures
        static_assert(
            false,
            "CheckPacketLost is not implemented for main_net futures yet");
        return true;  // либо логика сравнения update_id
    }
    void SetLastDiffUpdateId(uint64_t new_diff_update_id) override {
        static_assert(
            false,
            "SetLastDiffUpdateId is not implemented for main_net futures yet");
        OrderBookSyncBase<Price, Qty, MemoryPool,
                          HashMap>::last_diff_update_id_ = new_diff_update_id;
    }
    boost::asio::awaitable<void> RequestNewSnapshot(
        aos::TradingPair pair) override {
        boost::asio::io_context ioc;

        aoe::binance::impl::main_net::futures::RestSessionRW session(
            ioc, response_queue_, listener_);
        aoe::binance::snapshot::impl::SnapshotRequestSender<MemoryPool> sender(
            session);
        aoe::binance::snapshot::main_net::futures::impl::Request<MemoryPool>
            request;
        request.SetTradingPair(pair);
        // default depth = 5000;
        sender.Send(request);
        auto thread_ = std::jthread([&ioc]() { ioc.run(); });
        co_return;
    }
};
};  // namespace futures
};  // namespace main_net
};  // namespace impl
};  // namespace binance
};  // namespace aoe