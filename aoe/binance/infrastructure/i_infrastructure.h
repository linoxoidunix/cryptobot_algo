#pragma once
#include "aos/best_ask/best_ask.h"
#include "aos/best_bid/best_bid.h"
#include "aos/trading_pair/trading_pair.h"
namespace aoe {
namespace binance {
namespace spot {
namespace main_net {
class InfrastructureInterface {
  public:
    virtual ~InfrastructureInterface()                   = default;
    virtual void Register(aos::TradingPair trading_pair) = 0;
};

template <typename Price, typename Qty>
class InfrastructureNotifierOnBestBidChangeInterface {
  public:
    virtual ~InfrastructureNotifierOnBestBidChangeInterface() = default;
    /**
     * @brief Sets a callback to be invoked on best bid changes for a trading
     * pair.
     *
     * If a context exists for the specified trading pair, the callback will be
     * registered and the method returns true. Otherwise, no callback is
     * registered and the method returns false.
     *
     * @param cb The callback function to be called when the best bid changes.
     *           It receives the trading pair and the updated best bid as
     * arguments.
     * @return true if the context for the trading pair exists and the callback
     * was set; false otherwise.
     */
    virtual bool SetCallbackOnBestBidChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestBid<Price, Qty>& new_bid)> cb) = 0;
};

template <typename Price, typename Qty>
class InfrastructureNotifierOnBestAskChangeInterface {
  public:
    virtual ~InfrastructureNotifierOnBestAskChangeInterface() = default;
    /**
     * @brief Sets a callback to be invoked on best ask changes for a trading
     * pair.
     *
     * If a context exists for the specified trading pair, the callback will be
     * registered and the method returns true. Otherwise, no callback is
     * registered and the method returns false.
     *
     * @param cb The callback function to be called when the best ask changes.
     *           It receives the trading pair and the updated best ask as
     * arguments.
     * @return true if the context for the trading pair exists and the callback
     * was set; false otherwise.
     */
    virtual bool SetCallbackOnBestAskChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestAsk<Price, Qty>& new_ask)> cb) = 0;
};

template <typename Price, typename Qty>
class InfrastructureNotifierInterface
    : public InfrastructureNotifierOnBestBidChangeInterface<Price, Qty>,
      public InfrastructureNotifierOnBestAskChangeInterface<Price, Qty> {
  public:
    virtual ~InfrastructureNotifierInterface() = default;
};
};  // namespace main_net
};  // namespace spot
};  // namespace binance
};  // namespace aoe
