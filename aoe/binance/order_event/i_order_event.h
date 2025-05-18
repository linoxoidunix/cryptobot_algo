#pragma once
#include "aoe/binance/enums/enums.h"
#include "aoe/binance/order_mutator/i_order_mutator.h"
#include "aos/common/ref_counted.h"
#include "aos/order_event/i_order_event.h"
#include "aos/trading_pair/trading_pair.h"
#include "aot/common/types.h"

namespace aoe {
namespace binance {
template <template <typename> typename MemoryPool>
class OrderEventInterface : public aos::OrderEventInterface<MemoryPool> {
  public:
    virtual ~OrderEventInterface()                        = default;
    virtual aoe::binance::OrderStatus OrderStatus() const = 0;
    virtual double LeavesQty() const { return leaves_qty_; }
    virtual double LeavesValue() const { return leaves_value_; };
    virtual double CumExecQty() const { return cum_exec_qty_; };
    virtual double CumExecValue() const { return cum_exec_value_; };

    virtual common::ExchangeId ExchangeId() const { return exchange_id; };
    virtual aos::TradingPair TradingPair() const { return trading_pair_; };
    virtual double Price() const { return price_; };

    virtual void SetLeavesQty(double leaves_qty) { leaves_qty_ = leaves_qty; };
    virtual void SetLeavesValue(double leaves_value) {
        leaves_value_ = leaves_value;
    };
    virtual void SetCumExecQty(double cum_exec_qty) {
        cum_exec_qty_ = cum_exec_qty;
    };
    virtual void SetCumExecValue(double cum_exec_value) {
        cum_exec_value_ = cum_exec_value;
    };

    virtual void SetOrderId(uint64_t order_id) { order_id_ = order_id; };
    virtual void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    };
    virtual void SetPrice(double price) { price_ = price; };
    virtual void Accept(OrderMutatorInterface& order_storage) = 0;

  protected:
    aoe::binance::OrderStatus order_status_ =
        aoe::binance::OrderStatus::kInvalid;
    double leaves_qty_             = 0;
    double leaves_value_           = 0;
    double cum_exec_qty_           = 0;
    double cum_exec_value_         = 0;
    common::ExchangeId exchange_id = common::ExchangeId::kBybit;
    aos::TradingPair trading_pair_;
    uint64_t order_id_;
    double price_ = 0;
};
};  // namespace binance
};  // namespace aoe