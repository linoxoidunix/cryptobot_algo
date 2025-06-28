#pragma once
#include "aos/best_ask/best_ask.h"
#include "aos/best_bid/best_bid.h"
#include "aos/trading_pair/trading_pair.h"

namespace aoe::bybit::infrastructure {

class InfrastructureInterface {
  public:
    virtual ~InfrastructureInterface()                   = default;
    virtual void Register(aos::TradingPair trading_pair) = 0;
};

template <typename Price, typename Qty>
class NotifierOnBestBidChangeInterface {
  public:
    virtual ~NotifierOnBestBidChangeInterface() = default;

    virtual bool SetCallbackOnBestBidChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestBid<Price, Qty>& new_bid)> cb) = 0;
};

template <typename Price, typename Qty>
class NotifierOnBestAskChangeInterface {
  public:
    virtual ~NotifierOnBestAskChangeInterface() = default;

    virtual bool SetCallbackOnBestAskChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestAsk<Price, Qty>& new_ask)> cb) = 0;
};

template <typename Price, typename Qty>
class NotifierInterface : public NotifierOnBestBidChangeInterface<Price, Qty>,
                          public NotifierOnBestAskChangeInterface<Price, Qty> {
  public:
    ~NotifierInterface() override = default;
};

template <template <typename> typename MemoryPoolThreadSafety>
class OnPlaceOrderInterface {
  public:
    virtual ~OnPlaceOrderInterface() = default;

    /**
     * @brief Places a new order.
     *
     * @param request A pointer to the place order request.
     * @return `false` if no infrastructure exists for the specified trading
     * pair; `true` if the order was successfully passed to the handler
     *         (regardless of its eventual execution result).
     */
    virtual void PlaceOrder(
        boost::intrusive_ptr<
            aoe::bybit::place_order::RequestInterface<MemoryPoolThreadSafety>>
            request) = 0;
};

template <template <typename> typename MemoryPoolThreadSafety>
class OnCancelOrderInterface {
  public:
    virtual ~OnCancelOrderInterface() = default;

    /**
     * @brief Cancels an existing order.
     *
     * @param request A pointer to the cancel order request.
     * @return `false` if no infrastructure exists for the specified trading
     * pair; `true` if the cancel request was accepted for processing.
     */
    virtual void CancelOrder(
        boost::intrusive_ptr<
            aoe::bybit::cancel_order::RequestInterface<MemoryPoolThreadSafety>>
            request) = 0;
};

}  // namespace aoe::bybit::infrastructure

namespace aoe::bybit::spot::main_net {

using InfrastructureInterface =
    aoe::bybit::infrastructure::InfrastructureInterface;

template <typename Price, typename Qty>
using InfrastructureNotifierOnBestBidChangeInterface =
    aoe::bybit::infrastructure::NotifierOnBestBidChangeInterface<Price, Qty>;

template <typename Price, typename Qty>
using InfrastructureNotifierOnBestAskChangeInterface =
    aoe::bybit::infrastructure::NotifierOnBestAskChangeInterface<Price, Qty>;

template <typename Price, typename Qty>
using InfrastructureNotifierInterface =
    aoe::bybit::infrastructure::NotifierInterface<Price, Qty>;

template <template <typename> typename MemoryPoolThreadSafety>
using InfrastructurePlaceOrder =
    aoe::bybit::infrastructure::OnPlaceOrderInterface<MemoryPoolThreadSafety>;

template <template <typename> typename MemoryPoolThreadSafety>
using InfrastructureCancelOrder =
    aoe::bybit::infrastructure::OnCancelOrderInterface<MemoryPoolThreadSafety>;

}  // namespace aoe::bybit::spot::main_net

namespace aoe::bybit::linear::main_net {

using InfrastructureInterface =
    aoe::bybit::infrastructure::InfrastructureInterface;

template <typename Price, typename Qty>
using InfrastructureNotifierOnBestBidChangeInterface =
    aoe::bybit::infrastructure::NotifierOnBestBidChangeInterface<Price, Qty>;

template <typename Price, typename Qty>
using InfrastructureNotifierOnBestAskChangeInterface =
    aoe::bybit::infrastructure::NotifierOnBestAskChangeInterface<Price, Qty>;

template <typename Price, typename Qty>
using InfrastructureNotifierInterface =
    aoe::bybit::infrastructure::NotifierInterface<Price, Qty>;

// template <template <typename> typename MemoryPoolThreadSafety>
// using InfrastructurePlaceOrder =
//     aoe::bybit::infrastructure::OnPlaceOrderInterface<MemoryPoolThreadSafety>;

// template <template <typename> typename MemoryPoolThreadSafety>
// using InfrastructureCancelOrder =
//     aoe::bybit::infrastructure::OnCancelOrderInterface<MemoryPoolThreadSafety>;

}  // namespace aoe::bybit::linear::main_net
