#pragma once
#include <utility>  // std::pair

#include "aos/bbo_full/bbo_full.h"
#include "aos/best_ask/best_ask.h"
#include "aos/best_bid/best_bid.h"
#include "aos/order_book_event/i_order_book_event.h"
#include "boost/asio/awaitable.hpp"
#include "boost/intrusive_ptr.hpp"

namespace aos {
template <typename Price, typename Qty>
class OrderBookInnerInterface {
  public:
    // add bid level
    virtual void AddBidLevel(Price price, Qty qty) = 0;
    // remove bid lvl
    virtual void RemoveBidLevel(Price price)       = 0;
    // add ask level
    virtual void AddAskLevel(Price price, Qty qty) = 0;
    // remove ask lvl
    virtual void RemoveAskLevel(Price price)       = 0;
    // clear order book
    virtual void Clear()                           = 0;
    virtual ~OrderBookInnerInterface()             = default;
};

/**
 * @brief Interface for updating order book levels.
 *
 * This interface defines methods to update the top bid and ask levels
 * in an order book. It supports updating individual best levels, fixed-size
 * arrays of the top 5 levels, and dynamic vectors of arbitrary size.
 *
 * @tparam Price Type representing the price.
 * @tparam Qty Type representing the quantity.
 */
template <typename Price, typename Qty>
class TopLevelsExporterInterface {
  public:
    /**
     * @brief Update the best bid level.
     *
     * @param bid Reference to a BestBid object to be updated.
     * @return true if the best bid was updated, false otherwise.
     */
    virtual bool UpdateTopBid(BestBid<Price, Qty>& bid) const  = 0;

    /**
     * @brief Update only the best bid price.
     *
     * @param bid_price Reference to a Price variable to be updated with the
     * best bid price.
     * @return true if the bid price was updated, false otherwise.
     */
    virtual bool UpdateTopBidPrice(Price& bid_price_old) const = 0;

    /**
     * @brief Update the best ask level.
     *
     * @param ask Reference to a BestAsk object to be updated.
     * @return true if the best ask was updated, false otherwise.
     */
    virtual bool UpdateTopAsk(BestAsk<Price, Qty>& ask) const  = 0;

    /**
     * @brief Update only the best ask price.
     *
     * @param ask_price Reference to a Price variable to be updated with the
     * best ask price.
     * @return true if the ask price was updated, false otherwise.
     */
    virtual bool UpdateTopAskPrice(Price& ask_price_old) const = 0;

    /**
     * @brief Update the top 5 bid levels.
     *
     * @param array_5_bids Reference to a fixed-size array of 5 BestBid objects
     * to be updated.
     * @param max_valid_lvl Reference to a size_t that will hold the index of
     * the last updated level.
     * @return true if at least one bid level was updated, false otherwise.
     */
    virtual bool UpdateTop5Bids(
        std::array<BestBid<Price, Qty>, 5>& array_5_bids,
        std::size_t& max_valid_lvl) const = 0;

    /**
     * @brief Update the top 5 ask levels.
     *
     * @param array_5_asks Reference to a fixed-size array of 5 BestAsk objects
     * to be updated.
     * @param max_valid_lvl Reference to a size_t that will hold the index of
     * the last updated level.
     * @return true if at least one ask level was updated, false otherwise.
     */
    virtual bool UpdateTop5Asks(
        std::array<BestAsk<Price, Qty>, 5>& array_5_asks,
        std::size_t& max_valid_lvl) const                         = 0;

    /**
     * @brief Update the top N bid levels.
     *
     * @param n Number of top bid levels to update.
     * @param bids Reference to a vector of BestBid objects to be updated.
     *             The vector size must be at least n.
     * @param max_valid_lvl Reference to a size_t that will hold the index of
     * the last updated level.
     * @return true if at least one bid level was updated, false otherwise.
     *
     * @note The bids vector must have at least n elements; otherwise, the
     * function returns false and does not perform any updates.
     *
     * This requirement ensures the vector has enough capacity to hold the
     * top N bid levels. If the vector is smaller than n, the update is skipped
     * to avoid out-of-bounds memory access.
     */
    virtual bool UpdateTopNBids(std::size_t n,
                                std::vector<BestBid<Price, Qty>>& bids,
                                std::size_t& max_valid_lvl) const = 0;

    /**
     * @brief Update the top N ask levels.
     *
     * @param n Number of top ask levels to update.
     * @param asks Reference to a vector of BestAsk objects to be updated.
     *             The vector size must be at least n.
     * @param max_valid_lvl Reference to a size_t that will hold the index of
     * the last updated level.
     * @return true if at least one ask level was updated, false otherwise.
     *
     * @note The asks vector must have at least n elements; otherwise, the
     * function returns false and does not perform any updates.
     *
     * This ensures that the vector has enough capacity to store the top N ask
     * levels. If the vector is smaller than n, the update operation is skipped
     * to avoid out-of-bounds access.
     */
    virtual bool UpdateTopNAsks(std::size_t n,
                                std::vector<BestAsk<Price, Qty>>& asks,
                                std::size_t& max_valid_lvl) const = 0;

    /**
     * @brief Virtual destructor.
     */
    virtual ~TopLevelsExporterInterface()                         = default;
};

/**
 * @brief Async interface for updating order book levels.
 *
 * This interface defines methods to update the top bid and ask levels
 * in an order book. It supports updating individual best levels, fixed-size
 * arrays of the top 5 levels, and dynamic vectors of arbitrary size.
 *
 * @tparam Price Type representing the price.
 * @tparam Qty Type representing the quantity.
 */
template <typename Price, typename Qty>
class TopLevelsAsyncExporterInterface {
  public:
    virtual ~TopLevelsAsyncExporterInterface() = default;

    virtual boost::asio::awaitable<bool> UpdateTopBid(
        BestBid<Price, Qty>& bid) const = 0;

    virtual boost::asio::awaitable<bool> UpdateTopBidPrice(
        Price& bid_price) const = 0;

    virtual boost::asio::awaitable<bool> UpdateTopAsk(
        BestAsk<Price, Qty>& ask) const = 0;

    virtual boost::asio::awaitable<bool> UpdateTopAskPrice(
        Price& ask_price) const = 0;

    virtual boost::asio::awaitable<bool> UpdateTop5Bids(
        std::array<BestBid<Price, Qty>, 5>& array_5_bids,
        std::size_t& max_valid_lvl) const = 0;

    virtual boost::asio::awaitable<bool> UpdateTop5Asks(
        std::array<BestAsk<Price, Qty>, 5>& array_5_asks,
        std::size_t& max_valid_lvl) const = 0;

    virtual boost::asio::awaitable<bool> UpdateTopNBids(
        std::size_t n, std::vector<BestBid<Price, Qty>>& bids,
        std::size_t& max_valid_lvl) const = 0;

    virtual boost::asio::awaitable<bool> UpdateTopNAsks(
        std::size_t n, std::vector<BestAsk<Price, Qty>>& asks,
        std::size_t& max_valid_lvl) const = 0;
};

template <typename Price, typename Qty>
class HasBBOInterface {
  public:
    virtual std::pair<bool, BBOFull<Price, Qty>> GetBBO() = 0;
    virtual ~HasBBOInterface()                            = default;
};

class ClearOrderBookInterface {
  public:
    virtual void Clear()               = 0;
    virtual ~ClearOrderBookInterface() = default;
};

template <typename Price, typename Qty>
class OrderBookInterface : public ClearOrderBookInterface {
  public:
    // add bid level
    virtual void AddBidOrder(Price price, Qty qty) = 0;
    // remove bid lvl
    virtual void AddAskOrder(Price price, Qty qty) = 0;
    // remove ask lvl
    ~OrderBookInterface() override                 = default;
};

template <typename Price, typename Qty, template <typename> class MemoryPool>
class OrderBookEventListenerInterface : public ClearOrderBookInterface {
  public:
    // add bid level
    virtual void OnEvent(boost::intrusive_ptr<
                         OrderBookEventInterface<Price, Qty, MemoryPool>>) = 0;
    ~OrderBookEventListenerInterface() override = default;
};
};  // namespace aos