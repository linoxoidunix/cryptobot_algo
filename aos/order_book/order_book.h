#pragma once
#include "aos/order_book/i_order_book.h"
#include "aos/order_book_level/order_book_level.h"
#include "aos/order_book_subscriber/i_order_book_subscriber.h"
#include "aos/trading_pair/trading_pair.h"
#include "aot/common/types.h"
#include "boost/intrusive/avltree.hpp"
namespace aos {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename HashMap>
class OrderBookInner : public OrderBookInnerInterface<Price, Qty>,
                       public HasBBOInterface<Price, Qty>,
                       public TopLevelsExporterInterface<Price, Qty> {
    using MemberOption =
        boost::intrusive::member_hook<OrderBookLevel<Price, Qty>,
                                      boost::intrusive::avl_set_member_hook<>,
                                      &OrderBookLevel<Price, Qty>::member_hook>;
    using BidTree = boost::intrusive::avltree<
        OrderBookLevel<Price, Qty>,
        boost::intrusive::compare<std::greater<OrderBookLevel<Price, Qty>>>,
        MemberOption>;
    using AskTree = boost::intrusive::avltree<
        OrderBookLevel<Price, Qty>,
        boost::intrusive::compare<std::less<OrderBookLevel<Price, Qty>>>,
        MemberOption>;

    HashMap price_to_bid_level_;
    HashMap price_to_ask_level_;
    BidTree bid_levels_;
    AskTree ask_levels_;

    common::ExchangeId exchange_id_;
    aos::TradingPair trading_pair_;
    MemoryPool<OrderBookLevel<Price, Qty>> bid_lvl_memory_pool_;
    MemoryPool<OrderBookLevel<Price, Qty>> ask_lvl_memory_pool_;

  public:
    OrderBookInner(size_t max_level)
        : bid_lvl_memory_pool_(max_level), ask_lvl_memory_pool_(max_level) {};
    OrderBookInner(common::ExchangeId exchange_id,
                   aos::TradingPair trading_pair, size_t max_level)
        : exchange_id_(exchange_id),
          trading_pair_(trading_pair),
          bid_lvl_memory_pool_(max_level),
          ask_lvl_memory_pool_(max_level) {}
    ~OrderBookInner() override { Clear(); };
    void Clear() override {
        price_to_bid_level_.clear();
        price_to_ask_level_.clear();
        while (!bid_levels_.empty()) {
            auto it    = bid_levels_.begin();
            auto* node = &*it;
            bid_levels_.erase(it);
            bid_lvl_memory_pool_.Deallocate(node);
        }

        while (!ask_levels_.empty()) {
            auto it    = ask_levels_.begin();
            auto* node = &*it;
            ask_levels_.erase(it);
            ask_lvl_memory_pool_.Deallocate(node);
        }
    }
    void AddBidLevel(Price price, Qty qty) {
        logi("[ORDER_BOOK] add bid price:{} qty:{}", price, qty);
        auto contains = price_to_bid_level_.contains(price);
        if (contains) {
            price_to_bid_level_.at(price)->price = price;
            price_to_bid_level_.at(price)->qty   = qty;
            return;
        }
        auto ptr = bid_lvl_memory_pool_.Allocate(price, qty);
        price_to_bid_level_.insert({price, ptr});
        bid_levels_.insert_unique(*ptr);
    }
    void AddAskLevel(Price price, Qty qty) {
        logi("[ORDER_BOOK] add ask price:{} qty:{}", price, qty);
        auto contains = price_to_ask_level_.contains(price);
        if (contains) {
            price_to_ask_level_.at(price)->price = price;
            price_to_ask_level_.at(price)->qty   = qty;
            return;
        }
        auto ptr = ask_lvl_memory_pool_.Allocate(price, qty);
        price_to_ask_level_.insert({price, ptr});
        ask_levels_.insert_unique(*ptr);
    }
    void RemoveBidLevel(Price price) {
        logi("[ORDER_BOOK] rm bid price:{}", price);
        auto contains = price_to_bid_level_.contains(price);
        if (!contains) {
            return;
        }
        auto ptr = price_to_bid_level_.at(price);
        price_to_bid_level_.erase(price);
        bid_levels_.erase(*ptr);
        bid_lvl_memory_pool_.Deallocate(ptr);
    }
    void RemoveAskLevel(Price price) {
        logi("[ORDER_BOOK] rm ask price:{}", price);
        auto contains = price_to_ask_level_.contains(price);
        if (!contains) {
            return;
        }
        auto ptr = price_to_ask_level_.at(price);
        price_to_ask_level_.erase(price);
        ask_levels_.erase(*ptr);
        ask_lvl_memory_pool_.Deallocate(ptr);
    }
    std::pair<bool, BBOFull<Price, Qty>> GetBBO() override {
        if (bid_levels_.empty()) {
            return {false, {}};
        }
        if (ask_levels_.empty()) {
            return {false, {}};
        }
        BBOFull<Price, Qty> bbo;
        bbo.bid_price = bid_levels_.begin()->price;
        bbo.bid_qty   = bid_levels_.begin()->qty;
        bbo.ask_price = ask_levels_.begin()->price;
        bbo.ask_qty   = ask_levels_.begin()->qty;
        return std::make_pair(true, bbo);
    }
    bool UpdateTopBid(BestBid<Price, Qty>& bid) const override {
        bool update_exist = false;
        if (bid_levels_.empty()) return update_exist;
        auto first_bid = bid_levels_.begin();
        auto bid_price = first_bid->price;
        auto bid_qty   = first_bid->qty;
        if (bid.bid_price != bid_price || bid.bid_qty != bid_qty) {
            bid.bid_price = bid_price;
            bid.bid_qty   = bid_qty;
            update_exist  = true;
        }
        return update_exist;
    };
    bool UpdateTopAsk(BestAsk<Price, Qty>& ask) const override {
        bool update_exist = false;
        if (ask_levels_.empty()) return update_exist;
        auto first_ask = ask_levels_.begin();
        auto ask_price = first_ask->price;
        auto ask_qty   = first_ask->qty;
        if (ask.ask_price != ask_price || ask.ask_qty != ask_qty) {
            ask.ask_price = ask_price;
            ask.ask_qty   = ask_qty;
            update_exist  = true;
        }
        return update_exist;
    };
    bool UpdateTop5Bids(std::array<BestBid<Price, Qty>, 5>& array_5_bids,
                        std::size_t& max_valid_lvl) const override {
        std::size_t i     = 0;
        bool update_exist = false;
        for (auto it = bid_levels_.begin(); it != bid_levels_.end() && i < 5;
             ++it, ++i) {
            if (array_5_bids[i].bid_price != it->price ||
                array_5_bids[i].bid_qty != it->qty) {
                array_5_bids[i].bid_price = it->price;
                array_5_bids[i].bid_qty   = it->qty;
                update_exist              = true;
                max_valid_lvl             = i;
            }
        }
        return update_exist;
    };
    bool UpdateTop5Asks(std::array<BestAsk<Price, Qty>, 5>& array_5_asks,
                        std::size_t& max_valid_lvl) const override {
        std::size_t i     = 0;
        bool update_exist = false;
        for (auto it = ask_levels_.begin(); it != ask_levels_.end() && i < 5;
             ++it, ++i) {
            if (array_5_asks[i].ask_price != it->price ||
                array_5_asks[i].ask_qty != it->qty) {
                array_5_asks[i].ask_price = it->price;
                array_5_asks[i].ask_qty   = it->qty;
                update_exist              = true;
                max_valid_lvl             = i;
            }
        }
        return update_exist;
    };
    bool UpdateTopNBids(std::size_t n, std::vector<BestBid<Price, Qty>>& bids,
                        std::size_t& max_valid_lvl) const override {
        bool update_exist = false;
        if (bids.size() < n) {
            return false;  // Не обрабатываем, если текущий вектор меньше чем n
        }
        std::size_t i = 0;
        for (auto it = bid_levels_.begin(); it != bid_levels_.end() && i < n;
             ++it, ++i) {
            if (bids[i].bid_price != it->price || bids[i].bid_qty != it->qty) {
                bids[i].bid_price = it->price;
                bids[i].bid_qty   = it->qty;
                update_exist      = true;
                max_valid_lvl     = i;
            }
        }
        return update_exist;
    };
    bool UpdateTopNAsks(std::size_t n, std::vector<BestAsk<Price, Qty>>& asks,
                        std::size_t& max_valid_lvl) const override {
        bool update_exist = false;
        if (asks.size() < n) {
            return false;  // Не обрабатываем, если текущий вектор меньше чем n
        }
        std::size_t i = 0;
        for (auto it = ask_levels_.begin(); it != ask_levels_.end() && i < n;
             ++it, ++i) {
            if (asks[i].ask_price != it->price || asks[i].ask_qty != it->qty) {
                asks[i].ask_price = it->price;
                asks[i].ask_qty   = it->qty;
                update_exist      = true;
                max_valid_lvl     = i;
            }
        }
        return update_exist;
    };
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename HashMap>
class OrderBook : public OrderBookInterface<Price, Qty>,
                  public HasBBOInterface<Price, Qty>,
                  public TopLevelsExporterInterface<Price, Qty> {
    OrderBookInner<Price, Qty, MemoryPool, HashMap> inner_order_book_;

  public:
    OrderBook(size_t max_levels) : inner_order_book_(max_levels) {}
    OrderBook(common::ExchangeId exchange_id, aos::TradingPair trading_pair,
              size_t max_level)
        : inner_order_book_(exchange_id, trading_pair, max_level) {}
    void AddBidOrder(Price price, Qty qty) override {
        if (qty == 0) {
            inner_order_book_.RemoveBidLevel(price);
            return;
        }
        inner_order_book_.AddBidLevel(price, qty);
    };
    // remove bid lvl
    void AddAskOrder(Price price, Qty qty) override {
        if (qty == 0) {
            inner_order_book_.RemoveAskLevel(price);
            return;
        }
        inner_order_book_.AddAskLevel(price, qty);
    };
    void Clear() override { inner_order_book_.Clear(); };
    std::pair<bool, BBOFull<Price, Qty>> GetBBO() override {
        return inner_order_book_.GetBBO();
    }
    bool UpdateTopBid(BestBid<Price, Qty>& bid) const override {
        return inner_order_book_.UpdateTopBid(bid);
    };
    bool UpdateTopAsk(BestAsk<Price, Qty>& bid) const override {
        return inner_order_book_.UpdateTopAsk(bid);
    };
    bool UpdateTop5Bids(std::array<BestBid<Price, Qty>, 5>& array_5_bids,
                        std::size_t& max_valid_lvl) const override {
        return inner_order_book_.UpdateTop5Bids(array_5_bids, max_valid_lvl);
    };
    bool UpdateTop5Asks(std::array<BestAsk<Price, Qty>, 5>& array_5_asks,
                        std::size_t& max_valid_lvl) const override {
        return inner_order_book_.UpdateTop5Asks(array_5_asks, max_valid_lvl);
    };
    bool UpdateTopNBids(std::size_t n, std::vector<BestBid<Price, Qty>>& bids,
                        std::size_t& max_valid_lvl) const override {
        return inner_order_book_.UpdateTopNBids(n, bids, max_valid_lvl);
    };
    bool UpdateTopNAsks(std::size_t n, std::vector<BestAsk<Price, Qty>>& asks,
                        std::size_t& max_valid_lvl) const override {
        return inner_order_book_.UpdateTopNAsks(n, asks, max_valid_lvl);
    };

    ~OrderBook() = default;
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename HashMap>
class OrderBookEventListener
    : public OrderBookEventListenerInterface<Price, Qty, MemoryPool>,
      public TopLevelsAsyncExporterInterface<Price, Qty> {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    OrderBook<Price, Qty, MemoryPool, HashMap> order_book_;

  public:
    OrderBookEventListener(boost::asio::thread_pool& thread_pool,
                           size_t max_levels)
        : strand_(boost::asio::make_strand(thread_pool)),
          order_book_(max_levels) {}
    OrderBookEventListener(boost::asio::thread_pool& thread_pool,
                           common::ExchangeId exchange_id,
                           aos::TradingPair trading_pair, size_t max_level)
        : strand_(boost::asio::make_strand(thread_pool)),
          order_book_(exchange_id, trading_pair, max_level) {}
    ~OrderBookEventListener() override = default;
    void OnEvent(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) {
        boost::asio::co_spawn(strand_, ProcessEvent(ptr),
                              boost::asio::detached);
    }
    void Clear() override {
        boost::asio::co_spawn(strand_, ProcessClear(), boost::asio::detached);
    }
    boost::asio::awaitable<bool> UpdateTopBid(
        BestBid<Price, Qty>& bid) const override {
        co_await boost::asio::dispatch(strand_, boost::asio::use_awaitable);
        co_return order_book_.UpdateTopBid(bid);
    };
    boost::asio::awaitable<bool> UpdateTopAsk(
        BestAsk<Price, Qty>& bid) const override {
        co_await boost::asio::dispatch(strand_, boost::asio::use_awaitable);
        co_return order_book_.UpdateTopAsk(bid);
    };
    boost::asio::awaitable<bool> UpdateTop5Bids(
        std::array<BestBid<Price, Qty>, 5>& array_5_bids,
        std::size_t& max_valid_lvl) const override {
        co_await boost::asio::dispatch(strand_, boost::asio::use_awaitable);
        co_return order_book_.UpdateTop5Bids(array_5_bids, max_valid_lvl);
    };
    boost::asio::awaitable<bool> UpdateTop5Asks(
        std::array<BestAsk<Price, Qty>, 5>& array_5_asks,
        std::size_t& max_valid_lvl) const override {
        co_await boost::asio::dispatch(strand_, boost::asio::use_awaitable);
        co_return order_book_.UpdateTop5Asks(array_5_asks, max_valid_lvl);
    };
    boost::asio::awaitable<bool> UpdateTopNBids(
        std::size_t n, std::vector<BestBid<Price, Qty>>& bids,
        std::size_t& max_valid_lvl) const override {
        co_await boost::asio::dispatch(strand_, boost::asio::use_awaitable);
        co_return order_book_.UpdateTopNBids(n, bids, max_valid_lvl);
    };
    boost::asio::awaitable<bool> UpdateTopNAsks(
        std::size_t n, std::vector<BestAsk<Price, Qty>>& asks,
        std::size_t& max_valid_lvl) const override {
        co_await boost::asio::dispatch(strand_, boost::asio::use_awaitable);
        co_return order_book_.UpdateTopNAsks(n, asks, max_valid_lvl);
    };

  private:
    boost::asio::awaitable<void> ProcessEvent(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) {
        auto& bids = ptr->Bids();
        for (auto& it : bids) {
            order_book_.AddBidOrder(it.price, it.qty);
        }
        auto& asks = ptr->Asks();
        for (auto& it : asks) {
            order_book_.AddAskOrder(it.price, it.qty);
        }
        co_return;
    }
    boost::asio::awaitable<void> ProcessClear() {
        order_book_.Clear();
        co_return;
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookNotifier
    : public OrderBookEventListenerInterface<Price, Qty, MemoryPool> {
    OrderBookEventListenerInterface<Price, Qty, MemoryPool>& order_book_;
    std::vector<std::reference_wrapper<OrderBookSubscriberInterface>>
        subscribers_;

  public:
    OrderBookNotifier(
        OrderBookEventListenerInterface<Price, Qty, MemoryPool>& order_book)
        : order_book_(order_book) {}

    ~OrderBookNotifier() override = default;

    void OnEvent(
        boost::intrusive_ptr<OrderBookEventInterface<Price, Qty, MemoryPool>>
            ptr) override {
        order_book_.OnEvent(ptr);
        for (auto& sub : subscribers_) {
            sub.get().OnOrderBookUpdate();
        }
    }

    void Clear() override { order_book_.Clear(); }

    void AddSubscriber(OrderBookSubscriberInterface& sub) {
        subscribers_.emplace_back(sub);
    }

    void RemoveSubscriber(OrderBookSubscriberInterface& sub) {
        std::erase_if(subscribers_,
                      [&sub](auto& ref) { return &ref.get() == &sub; });
    }
};

};  // namespace aos
