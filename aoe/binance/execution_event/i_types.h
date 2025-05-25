#pragma once
#include "aos/common/ref_counted.h"
#include "aos/position_storage/position_storage_by_pair/i_position_storage_by_pair.h"
#include "aos/trading_pair/trading_pair.h"
#include "aot/common/types.h"

namespace aoe {
namespace binance {
template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventInterface
    : public common::RefCounted<
          MemoryPool, ExecutionEventInterface<MemoryPool, PositionT>> {
  public:
    virtual ~ExecutionEventInterface() = default;
    virtual double Fee() const { return fee_; }
    virtual double ExecPrice() const { return exec_price_; }
    virtual double ExecQty() const = 0;
    virtual double ExecQtyAbs() const { return exec_qty_; }
    virtual double ExecValue() const { return exec_value_; }
    // client order id
    virtual uint64_t OrderId() const { return order_id_; };
    virtual common::ExchangeId ExchangeId() const { return exchange_id_; };
    virtual aos::TradingPair TradingPair() const { return trading_pair_; };

    virtual void SetFee(double fee) { fee_ = fee; };
    virtual void SetExecPrice(double exec_price) { exec_price_ = exec_price; };
    virtual void SetExecQty(double exec_qty) { exec_qty_ = exec_qty; };
    virtual void SetExecValue(double exec_value) { exec_value_ = exec_value; };
    virtual void SetOrderId(uint64_t order_id) { order_id_ = order_id; };
    virtual void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    };

    virtual void Accept(
        aos::PositionStorageByPairInterface<double, double, PositionT>&
            position_storage) = 0;

  protected:
    double fee_                     = 0;
    double exec_price_              = 0;
    double exec_qty_                = 0;
    double exec_value_              = 0;
    uint64_t order_id_              = 0;
    common::ExchangeId exchange_id_ = common::ExchangeId::kBybit;
    aos::TradingPair trading_pair_;
};
};  // namespace binance
};  // namespace aoe