#pragma once
#include "aos/best_ask/best_ask.h"
#include "aos/best_bid/best_bid.h"
#include "aos/trading_pair/trading_pair.h"

namespace aoe {
namespace bybit {

// Общий базовый интерфейс инфраструктуры
class InfrastructureInterface {
  public:
    virtual ~InfrastructureInterface()                   = default;
    virtual void Register(aos::TradingPair trading_pair) = 0;
    virtual void StartAsync()                            = 0;
    virtual void StopAsync()                             = 0;
};

// Шаблонный интерфейс уведомлений по изменению best bid/ask
template <typename Price, typename Qty>
class InfrastructureNotifierOnBestBidChangeInterface {
  public:
    virtual ~InfrastructureNotifierOnBestBidChangeInterface() = default;
    virtual bool SetCallbackOnBestBidChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestBid<Price, Qty>& new_bid)> cb) = 0;
};

template <typename Price, typename Qty>
class InfrastructureNotifierOnBestAskChangeInterface {
  public:
    virtual ~InfrastructureNotifierOnBestAskChangeInterface() = default;
    virtual bool SetCallbackOnBestAskChange(
        aos::TradingPair trading_pair,
        std::function<void(aos::BestAsk<Price, Qty>& new_ask)> cb) = 0;
};

template <typename Price>
class InfrastructureNotifierOnBestBidPriceChangeInterface {
  public:
    virtual ~InfrastructureNotifierOnBestBidPriceChangeInterface() = default;
    virtual bool SetCallbackOnBestBidPriceChange(
        aos::TradingPair trading_pair,
        std::function<void(Price& new_bid_price)> cb) = 0;
};

template <typename Price>
class InfrastructureNotifierOnBestAskPriceChangeInterface {
  public:
    virtual ~InfrastructureNotifierOnBestAskPriceChangeInterface() = default;
    virtual bool SetCallbackOnBestAskPriceChange(
        aos::TradingPair trading_pair,
        std::function<void(Price& new_ask_price)> cb) = 0;
};

// Объединённый интерфейс уведомлений
template <typename Price, typename Qty>
class InfrastructureNotifierInterface
    : public InfrastructureNotifierOnBestBidChangeInterface<Price, Qty>,
      public InfrastructureNotifierOnBestAskChangeInterface<Price, Qty>,
      public InfrastructureNotifierOnBestBidPriceChangeInterface<Price>,
      public InfrastructureNotifierOnBestAskPriceChangeInterface<Price> {
  public:
    ~InfrastructureNotifierInterface() override = default;
};

// Пространство имён для Spot и Futures с alias-ами
namespace spot {
namespace main_net {
using Infrastructure = InfrastructureInterface;
template <typename Price, typename Qty>
using InfrastructureNotifierInterface =
    InfrastructureNotifierInterface<Price, Qty>;
}  // namespace main_net
}  // namespace spot

namespace linear {
namespace main_net {
using Infrastructure = InfrastructureInterface;
template <typename Price, typename Qty>
using InfrastructureNotifierInterface =
    InfrastructureNotifierInterface<Price, Qty>;
}  // namespace main_net
}  // namespace linear

}  // namespace bybit
}  // namespace aoe
