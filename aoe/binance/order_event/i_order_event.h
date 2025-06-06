#pragma once
#include "aoe/binance/enums/enums.h"
#include "aoe/binance/order_mutator/i_order_mutator.h"
#include "aos/common/ref_counted.h"
#include "aos/order_event/i_order_event.h"
#include "aos/trading_pair/trading_pair.h"
#include "aos/common/exchange_id.h"

namespace aoe {
namespace binance {
template <template <typename> typename MemoryPool>
class OrderEventInterface : public aos::OrderEventInterface<MemoryPool> {
  public:
    virtual ~OrderEventInterface() = default;
    virtual void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    };
    virtual aos::TradingPair TradingPair() { return trading_pair_; };

    virtual aoe::binance::OrderStatus OrderStatus() const {
        return order_status_;
    };
    virtual void SetOrderStatus(aoe::binance::OrderStatus order_status) {
        order_status_ = order_status;
    };

    virtual void SetOrderId(uint64_t order_id) { order_id_ = order_id; };
    virtual uint64_t OrderId() const { return order_id_; };

    virtual void SetPrice(double price) { price_ = price; };
    virtual double Price() const { return price_; };

    virtual void SetCumExecQty(double cum_exec_qty) {
        cum_exec_qty_ = cum_exec_qty;
    };
    virtual double CumExecQty() const { return cum_exec_qty_; };

    virtual void SetOrderQty(double order_qty) { order_qty_ = order_qty; };
    virtual double OrderQty() const { return order_qty_; };

    virtual void Accept(OrderMutatorInterface& order_storage) = 0;

  protected:
    common::ExchangeId exchange_id = common::ExchangeId::kBinance;
    aos::TradingPair trading_pair_;
    aoe::binance::OrderStatus order_status_ =
        aoe::binance::OrderStatus::kInvalid;
    double price_ = 0;
    uint64_t order_id_;
    double order_qty_    = 0;
    // how much executed qty
    double cum_exec_qty_ = 0;
};
};  // namespace binance
};  // namespace aoe