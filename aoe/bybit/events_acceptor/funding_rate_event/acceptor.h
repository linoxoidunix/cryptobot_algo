#pragma once
#include "aoe/bybit/events_acceptor/funding_rate_event/i_acceptor.h"
#include "aos/logger/mylog.h"
#include "aos/order_book/i_order_book.h"
#include "boost/asio.hpp"

namespace aoe {
namespace bybit {
namespace impl {
namespace event_acceptor {
namespace funding_rate_event {
template <template <typename> typename MemoryPoolThreadSafety>
class Acceptor
    : public FundingRateEventAcceptorInterface<MemoryPoolThreadSafety> {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    // aos::OrderBookEventListenerInterface<MemoryPoolThreadSafety>&
    // order_book_;//entity to process current funding rate and funding next
    // time
    uint64_t current_diff_update_id_;
    uint64_t current_snapshot_update_id_;
    bool received_snapshot_success_ = false;

  public:
    explicit Acceptor(boost::asio::thread_pool& thread_pool
                      /*need pass entity to process funding rate event*/)
        : strand_(boost::asio::make_strand(thread_pool)) {}
    ~Acceptor() override = default;
    void OnEvent(
        boost::intrusive_ptr<FundingRateEventInterface<MemoryPoolThreadSafety>>
            ptr) override {
        boost::asio::post(strand_, [ptr = std::move(ptr)] {
            logi("{}, funding_rate:{}, next_funding_time:{}",
                 ptr->TradingPair(), ptr->FundingRate(),
                 ptr->NextFundingTime());
        });
    };
};
};  // namespace funding_rate_event
};  // namespace event_acceptor
};  // namespace impl
};  // namespace bybit
};  // namespace aoe