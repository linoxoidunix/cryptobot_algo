#pragma once
#include "aos/best_ask/best_ask.h"
#include "aos/best_bid/best_bid.h"
#include "aos/order_book/i_order_book.h"
#include "aos/order_book_subscriber/i_order_book_subscriber.h"
#include "boost/asio.hpp"

namespace aos {
namespace impl {
template <typename Price, typename Qty>
class BestBidBestAsk : public OrderBookSubscriberInterface {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter_;
    BestBid<Price, Qty> best_bid_;
    BestAsk<Price, Qty> best_ask_;

  public:
    BestBidBestAsk(
        boost::asio::thread_pool& thread_pool,
        TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter)
        : strand_(boost::asio::make_strand(thread_pool)),
          top_level_async_exporter_(top_level_async_exporter) {}
    void OnOrderBookUpdate() override {
        boost::asio::co_spawn(strand_, ProcessOnOrderBookUpdate(),
                              boost::asio::detached);
    }

  private:
    boost::asio::awaitable<void> ProcessOnOrderBookUpdate() {
        auto update_bid_exist =
            top_level_async_exporter_.UpdateTopBid(best_bid_);
        auto update_ask_exist =
            top_level_async_exporter_.UpdateTopAsk(best_ask_);
        if (update_bid_exist && update_ask_exist) {
            logi("[BestBidBestAsk] send new signal to My realized strategy");
        }
        co_return;
    }
};
};  // namespace impl
};  // namespace aos