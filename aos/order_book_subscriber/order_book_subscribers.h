#pragma once
#include "aos/best_ask/best_ask.h"
#include "aos/best_bid/best_bid.h"
#include "aos/order_book/i_order_book.h"
#include "aos/order_book_subscriber/i_order_book_subscriber.h"
#include "boost/asio.hpp"

namespace aos {
namespace impl {
template <typename Price, typename Qty>
class BestBidOrBestAskNotifier : public OrderBookSubscriberInterface {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter_;
    BestBid<Price, Qty> best_bid_;
    BestAsk<Price, Qty> best_ask_;

  public:
    BestBidOrBestAskNotifier(
        boost::asio::thread_pool& thread_pool,
        TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter)
        : strand_(boost::asio::make_strand(thread_pool)),
          top_level_async_exporter_(top_level_async_exporter) {}
    void OnOrderBookUpdate() override {
        boost::asio::co_spawn(strand_, ProcessOnOrderBookUpdate(),
                              boost::asio::detached);
    }
    ~BestBidOrBestAskNotifier() override = default;

  private:
    boost::asio::awaitable<void> ProcessOnOrderBookUpdate() {
        auto old_bid = best_bid_;
        auto old_ask = best_ask_;

        auto update_bid_exist =
            co_await top_level_async_exporter_.UpdateTopBid(best_bid_);
        auto update_ask_exist =
            co_await top_level_async_exporter_.UpdateTopAsk(best_ask_);

        if (update_bid_exist || update_ask_exist) {
            logi(
                "[BestBidOrBestAskNotifier] send new signal to My realized "
                "strategy");
            logi("  Best Bid Possible Changed:");
            logi("    Old: price={}, qty={}", old_bid.bid_price,
                 old_bid.bid_qty);
            logi("    New: price={}, qty={}", best_bid_.bid_price,
                 best_bid_.bid_qty);

            logi("  Best Ask Possible Changed:");
            logi("    Old: price={}, qty={}", old_ask.ask_price,
                 old_ask.ask_qty);
            logi("    New: price={}, qty={}", best_ask_.ask_price,
                 best_ask_.ask_qty);
        }
        co_return;
    }
};

template <typename Price, typename Qty>
class BestBidNotifier : public OrderBookSubscriberInterface {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter_;
    BestBid<Price, Qty> best_bid_;

  public:
    BestBidNotifier(
        boost::asio::thread_pool& thread_pool,
        TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter)
        : strand_(boost::asio::make_strand(thread_pool)),
          top_level_async_exporter_(top_level_async_exporter) {}
    void OnOrderBookUpdate() override {
        boost::asio::co_spawn(strand_, ProcessOnOrderBookUpdate(),
                              boost::asio::detached);
    }
    ~BestBidNotifier() override = default;

  private:
    boost::asio::awaitable<void> ProcessOnOrderBookUpdate() {
        auto old_bid = best_bid_;
        auto update_bid_exist =
            co_await top_level_async_exporter_.UpdateTopBid(best_bid_);
        if (update_bid_exist) {
            logi("[BestBidNotifier] Best bid updated:");
            logi("    Old: price={}, qty={}", old_bid.bid_price,
                 old_bid.bid_qty);
            logi("    New: price={}, qty={}", best_bid_.bid_price,
                 best_bid_.bid_qty);
            logi("    Sending signal to strategy.");
        }
        co_return;
    }
};

template <typename Price, typename Qty>
class BestAskNotifier : public OrderBookSubscriberInterface {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter_;
    BestAsk<Price, Qty> best_ask_;

  public:
    BestAskNotifier(
        boost::asio::thread_pool& thread_pool,
        TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter)
        : strand_(boost::asio::make_strand(thread_pool)),
          top_level_async_exporter_(top_level_async_exporter) {}
    void OnOrderBookUpdate() override {
        boost::asio::co_spawn(strand_, ProcessOnOrderBookUpdate(),
                              boost::asio::detached);
    }
    ~BestAskNotifier() override = default;

  private:
    boost::asio::awaitable<void> ProcessOnOrderBookUpdate() {
        auto old_ask = best_ask_;
        auto update_bid_exist =
            co_await top_level_async_exporter_.UpdateTopAsk(best_ask_);
        if (update_bid_exist) {
            logi("[BestAskNotifier] Best ask updated:");
            logi("    Old: price={}, qty={}", old_ask.ask_price,
                 old_ask.ask_qty);
            logi("    New: price={}, qty={}", best_ask_.ask_price,
                 best_ask_.ask_qty);
            logi("    Sending signal to strategy.");
        }
        co_return;
    }
};
};  // namespace impl
};  // namespace aos