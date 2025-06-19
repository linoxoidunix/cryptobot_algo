#pragma once
#include <queue>

#include "aoe/binance/constants/constants.h"
#include "aoe/binance/order_book_sync/i_order_book_sync.h"
#include "aoe/binance/parser/json/rest/snapshot_response/parser.h"
#include "aoe/binance/response_queue_listener/json/rest/snapshot/listener.h"
#include "aoe/binance/rest_request_sender/rest_request_sender.h"
#include "aoe/binance/session/rest/session.h"
#include "aos/order_book/i_order_book.h"
// #include "aos/order_book/order_book.h"
#include "aos/logger/mylog.h"
#include "boost/asio.hpp"

namespace aoe {
namespace binance {
namespace impl {
namespace main_net {
namespace spot {

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename HashMap>
class OrderBookSync
    : public aoe::binance::spot::OrderBookSyncInterface<Price, Qty,
                                                        MemoryPool> {
    aoe::binance::impl::SnapshotEventParser<Price, Qty, MemoryPool>
        snapshot_parser_{kMaximumSnapshotEventsFromExchange};
    moodycamel::ConcurrentQueue<std::vector<char>> response_queue_;
    aoe::binance::impl::snapshot::spot::Listener<Price, Qty, MemoryPool>
        listener_;

  protected:
    boost::asio::thread_pool& thread_pool_;
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    aos::OrderBookEventListenerInterface<Price, Qty, MemoryPool>& order_book_;
    uint64_t last_diff_update_id_       = 0;
    uint64_t last_snapshot_update_id_   = 0;

    bool need_make_snapshot_            = true;
    bool need_process_current_snapshot_ = false;
    bool diff_packet_lost_              = true;
    bool snapshot_request_in_flight_    = false;
    bool snapshot_was_processed_        = false;
    boost::intrusive_ptr<
        OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
        last_snapshot_;
    std::queue<
        boost::intrusive_ptr<aoe::binance::spot::OrderBookDiffEventInterface<
            Price, Qty, MemoryPool>>>
        diffs_;

  public:
    OrderBookSync(boost::asio::thread_pool& thread_pool,
                  aos::OrderBookEventListenerInterface<Price, Qty, MemoryPool>&
                      order_book)
        : listener_(thread_pool, response_queue_, snapshot_parser_, *this),
          thread_pool_(thread_pool),
          strand_(boost::asio::make_strand(thread_pool)),
          order_book_(order_book) {}

    void AcceptSnapshot(boost::intrusive_ptr<
                        OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
                            ptr) override {
        boost::asio::co_spawn(strand_, ProcessSnapshot(ptr),
                              boost::asio::detached);
    }

    void AcceptDiff(
        boost::intrusive_ptr<aoe::binance::spot::OrderBookDiffEventInterface<
            Price, Qty, MemoryPool>>
            ptr) override {
        boost::asio::co_spawn(strand_, ProcessDiff(ptr), boost::asio::detached);
    }

  protected:
    bool CheckPacketLost(boost::intrusive_ptr<
                         OrderBookDiffEventInterface<Price, Qty, MemoryPool>>
                             diff) {
        // реализация для main_net spot
        logd("diff_first_id:{} last_diff_update_id:{}", diff->FirstUpdateId(),
             OrderBookSync<Price, Qty, MemoryPool,
                           HashMap>::last_diff_update_id_);

        return (diff->FirstUpdateId() !=
                OrderBookSync<Price, Qty, MemoryPool,
                              HashMap>::last_diff_update_id_ +
                    1);
    }
    void SetLastDiffUpdateId(uint64_t new_diff_update_id) {
        OrderBookSync<Price, Qty, MemoryPool, HashMap>::last_diff_update_id_ =
            new_diff_update_id;
    }
    boost::asio::awaitable<void> RequestNewSnapshot(aos::TradingPair pair) {
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
        auto thread = std::jthread([&ioc]() { ioc.run(); });
        co_return;
    }
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
        need_process_current_snapshot_ = true;
        co_return;
    }
    boost::asio::awaitable<void> ProcessDiff(
        boost::intrusive_ptr<aoe::binance::spot::OrderBookDiffEventInterface<
            Price, Qty, MemoryPool>>
            ptr) {
        logd("Received diff: FirstUpdateId={}, FinalUpdateId={} {}",
             ptr->FirstUpdateId(), ptr->FinalUpdateId(), ptr->TradingPair());
        // проверка что следующий дифф равен
        if (snapshot_was_processed_) {
            bool packet_lost = CheckPacketLost(ptr);
            logd("packet lost:{}", packet_lost);
            if (!packet_lost) {
                order_book_.OnEvent(ptr);
                SetLastDiffUpdateId(ptr->FinalUpdateId());
                co_return;
            }
        }
        diffs_.push(ptr);
        if (need_make_snapshot_) {
            if (!snapshot_request_in_flight_) {
                order_book_.Clear();
                logi("Requesting new snapshot for pair {}", ptr->TradingPair());
                snapshot_request_in_flight_ = true;
                boost::asio::co_spawn(thread_pool_,
                                      RequestNewSnapshot(ptr->TradingPair()),
                                      boost::asio::detached);
                need_make_snapshot_ = false;
                co_return;
            }
        }
        if (snapshot_request_in_flight_) {
            co_return;
        }

        if (need_process_current_snapshot_) {
            need_process_current_snapshot_ = false;
            logi("Applying last snapshot to order book for {}",
                 ptr->TradingPair());
            order_book_.OnEvent(last_snapshot_);
            snapshot_was_processed_ = true;
        }
        bool need_clear_buffer_diffs = false;
        while (!diffs_.empty()) {
            auto first_diff   = diffs_.front();
            diff_packet_lost_ = CheckPacketLost(first_diff);
            logd("Packet lost check result: {}", diff_packet_lost_);
            if (diff_packet_lost_) {
                // need skip next;
                // need skip current buffer diffs
                need_make_snapshot_     = true;
                snapshot_was_processed_ = false;
                need_clear_buffer_diffs = true;
                break;
            }
            SetLastDiffUpdateId(first_diff->FinalUpdateId());
            logd("Set last diff update id to {}", first_diff->FinalUpdateId());
            diffs_.pop();
            order_book_.OnEvent(first_diff);
        }
        if (need_clear_buffer_diffs) {
            while (!diffs_.empty()) {
                diffs_.pop();
            }
        }
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
    : public aoe::binance::futures::OrderBookSyncInterface<Price, Qty,
                                                           MemoryPool> {
    aoe::binance::impl::SnapshotEventParser<Price, Qty, MemoryPool>
        snapshot_parser_{kMaximumSnapshotEventsFromExchange};
    moodycamel::ConcurrentQueue<std::vector<char>> response_queue_;
    aoe::binance::impl::snapshot::futures::Listener<Price, Qty, MemoryPool>
        listener_;

  protected:
    boost::asio::thread_pool& thread_pool_;
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    aos::OrderBookEventListenerInterface<Price, Qty, MemoryPool>& order_book_;
    uint64_t last_diff_update_id_       = 0;
    uint64_t last_snapshot_update_id_   = 0;

    bool need_make_snapshot_            = true;
    bool need_process_current_snapshot_ = false;
    bool diff_packet_lost_              = true;
    bool snapshot_request_in_flight_    = false;
    bool snapshot_was_processed_        = false;
    boost::intrusive_ptr<
        OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
        last_snapshot_;
    std::queue<
        boost::intrusive_ptr<aoe::binance::futures::OrderBookDiffEventInterface<
            Price, Qty, MemoryPool>>>
        diffs_;

  public:
    OrderBookSync(boost::asio::thread_pool& thread_pool,
                  aos::OrderBookEventListenerInterface<Price, Qty, MemoryPool>&
                      order_book)
        : listener_(thread_pool, response_queue_, snapshot_parser_, *this),
          thread_pool_(thread_pool),
          strand_(boost::asio::make_strand(thread_pool)),
          order_book_(order_book) {}

    void AcceptSnapshot(boost::intrusive_ptr<
                        OrderBookSnapshotEventInterface<Price, Qty, MemoryPool>>
                            ptr) override {
        boost::asio::co_spawn(strand_, ProcessSnapshot(ptr),
                              boost::asio::detached);
    }

    void AcceptDiff(
        boost::intrusive_ptr<aoe::binance::futures::OrderBookDiffEventInterface<
            Price, Qty, MemoryPool>>
            ptr) override {
        boost::asio::co_spawn(strand_, ProcessDiff(ptr), boost::asio::detached);
    }

  protected:
    bool CheckPacketLost(
        boost::intrusive_ptr<aoe::binance::futures::OrderBookDiffEventInterface<
            Price, Qty, MemoryPool>>
            diff) {
        // реализация для main_net futures
        logd("diff_first_id:{} last_diff_update_id:{}",
             diff->PreviousUpdateId(),
             OrderBookSync<Price, Qty, MemoryPool,
                           HashMap>::last_diff_update_id_);

        return (diff->PreviousUpdateId() !=
                OrderBookSync<Price, Qty, MemoryPool,
                              HashMap>::last_diff_update_id_);
    }
    void SetLastDiffUpdateId(uint64_t new_diff_update_id) {
        OrderBookSync<Price, Qty, MemoryPool, HashMap>::last_diff_update_id_ =
            new_diff_update_id;
    }
    boost::asio::awaitable<void> RequestNewSnapshot(aos::TradingPair pair) {
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
        auto thread = std::jthread([&ioc]() { ioc.run(); });
        co_return;
    }
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
        need_process_current_snapshot_ = true;
        co_return;
    }
    boost::asio::awaitable<void> ProcessDiff(
        boost::intrusive_ptr<aoe::binance::futures::OrderBookDiffEventInterface<
            Price, Qty, MemoryPool>>
            ptr) {
        logd("Received diff: FirstUpdateId={}, FinalUpdateId={} {}",
             ptr->FirstUpdateId(), ptr->FinalUpdateId(), ptr->TradingPair());
        // проверка что следующий дифф равен
        if (snapshot_was_processed_) {
            bool packet_lost = CheckPacketLost(ptr);
            logd("packet lost:{}", packet_lost);
            if (!packet_lost) {
                order_book_.OnEvent(ptr);
                SetLastDiffUpdateId(ptr->FinalUpdateId());
                co_return;
            }
        }
        diffs_.push(ptr);
        if (need_make_snapshot_) {
            if (!snapshot_request_in_flight_) {
                order_book_.Clear();
                logi("Requesting new snapshot for pair {}", ptr->TradingPair());
                snapshot_request_in_flight_ = true;
                boost::asio::co_spawn(thread_pool_,
                                      RequestNewSnapshot(ptr->TradingPair()),
                                      boost::asio::detached);
                need_make_snapshot_ = false;
                co_return;
            }
        }
        if (snapshot_request_in_flight_) {
            co_return;
        }

        if (need_process_current_snapshot_) {
            need_process_current_snapshot_ = false;
            logi("Applying last snapshot to order book for {}",
                 ptr->TradingPair());
            order_book_.OnEvent(last_snapshot_);
            snapshot_was_processed_ = true;
        }
        bool need_clear_buffer_diffs = false;
        while (!diffs_.empty()) {
            auto first_diff   = diffs_.front();
            diff_packet_lost_ = CheckPacketLost(first_diff);
            logd("Packet lost check result: {}", diff_packet_lost_);
            if (diff_packet_lost_) {
                // need skip next;
                // need skip current buffer diffs
                need_make_snapshot_     = true;
                snapshot_was_processed_ = false;
                need_clear_buffer_diffs = true;
                break;
            }
            SetLastDiffUpdateId(first_diff->FinalUpdateId());
            logd("Set last diff update id to {}", first_diff->FinalUpdateId());
            diffs_.pop();
            order_book_.OnEvent(first_diff);
        }
        if (need_clear_buffer_diffs) {
            while (!diffs_.empty()) {
                diffs_.pop();
            }
        }
        co_return;
    }
};
};  // namespace futures
};  // namespace main_net
};  // namespace impl
};  // namespace binance
};  // namespace aoe