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
class BestBidNotifier : public BestBidNotifierInterface<Price, Qty> {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter_;
    BestBid<Price, Qty> best_bid_;
    std::function<void(BestBid<Price, Qty>& new_bid)>
        on_best_bid_update_callback_;

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
    void SetCallback(
        std::function<void(BestBid<Price, Qty>& new_bid)> cb) override {
        on_best_bid_update_callback_ = cb;
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
            logi("[BestBidNotifier] invoke custom callback:");
            on_best_bid_update_callback_(best_bid_);
        }
        co_return;
    }
};

template <typename Price, typename Qty>
class BestAskNotifier : public BestAskNotifierInterface<Price, Qty> {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter_;
    BestAsk<Price, Qty> best_ask_;
    std::function<void(BestAsk<Price, Qty>& new_ask)>
        on_best_ask_update_callback_;

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
    void SetCallback(
        std::function<void(BestAsk<Price, Qty>& new_ask)> cb) override {
        on_best_ask_update_callback_ = cb;
    };
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
            logi("[BestAskNotifier] invoke custom callback:");
            on_best_ask_update_callback_(best_ask_);
        }
        co_return;
    }
};

template <typename Price, typename Qty>
class BestBidPriceNotifier : public BestBidPriceNotifierInterface<Price> {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter_;
    Price best_bid_price_;
    std::function<void(Price& new_bid_price)>
        on_best_bid_price_update_callback_;

  public:
    BestBidPriceNotifier(
        boost::asio::thread_pool& thread_pool,
        TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter)
        : strand_(boost::asio::make_strand(thread_pool)),
          top_level_async_exporter_(top_level_async_exporter) {}
    void OnOrderBookUpdate() override {
        boost::asio::co_spawn(strand_, ProcessOnOrderBookUpdate(),
                              boost::asio::detached);
    }
    void SetCallback(std::function<void(Price& new_bid_price)> cb) override {
        on_best_bid_price_update_callback_ = cb;
    }
    ~BestBidPriceNotifier() override = default;

  private:
    boost::asio::awaitable<void> ProcessOnOrderBookUpdate() {
        auto best_bid_price_old = best_bid_price_;
        auto update_bid_price_exist =
            co_await top_level_async_exporter_.UpdateTopBidPrice(
                best_bid_price_);
        if (update_bid_price_exist) {
            logi("[BestBidPriceNotifier] Best bid price updated:");
            logi("    Old: price={}", best_bid_price_old);
            logi("    New: price={}", best_bid_price_);
            logi("    Sending signal to strategy.");
            logi("[BestBidPriceNotifier] invoke custom callback:");
            on_best_bid_price_update_callback_(best_bid_price_);
        }
        co_return;
    }
};
template <typename Price, typename Qty>
class BestAskPriceNotifier : public BestAskPriceNotifierInterface<Price> {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter_;
    Price best_ask_price_;
    std::function<void(Price& new_ask_price)>
        on_best_ask_price_update_callback_;

  public:
    BestAskPriceNotifier(
        boost::asio::thread_pool& thread_pool,
        TopLevelsAsyncExporterInterface<Price, Qty>& top_level_async_exporter)
        : strand_(boost::asio::make_strand(thread_pool)),
          top_level_async_exporter_(top_level_async_exporter) {}
    void OnOrderBookUpdate() override {
        boost::asio::co_spawn(strand_, ProcessOnOrderBookUpdate(),
                              boost::asio::detached);
    }
    void SetCallback(std::function<void(Price& new_ask_price)> cb) override {
        on_best_ask_price_update_callback_ = cb;
    };
    ~BestAskPriceNotifier() override = default;

  private:
    boost::asio::awaitable<void> ProcessOnOrderBookUpdate() {
        auto ask_price_old = best_ask_price_;
        auto update_ask_price_exist =
            co_await top_level_async_exporter_.UpdateTopAskPrice(
                best_ask_price_);
        if (update_ask_price_exist) {
            logi("[BestAskPriceNotifier] Best ask price updated:");
            logi("    Old: price={}", ask_price_old);
            logi("    New: price={}", best_ask_price_);
            logi("    Sending signal to strategy.");
            logi("[BestAskPriceNotifier] invoke custom callback:");
            on_best_ask_price_update_callback_(best_ask_price_);
        }
        co_return;
    }
};
};  // namespace impl
};  // namespace aos