#pragma once
#include "aoe/bybit/enums/enums.h"
#include "aos/common/common.h"
#include "aos/order_types/i_order_type.h"
#include "aot/common/types.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class OrderTypeInterface : public aos::OrderTypeInterface<MemoryPool> {
  public:
    virtual ~OrderTypeInterface() = default;
    virtual common::ExchangeId ExchangeId() const { return exchange_id_; };
    virtual common::TradingPair TradingPair() const { return trading_pair_; };
    virtual aoe::bybit::Category Category() const { return category_; };
    virtual double Price() const { return price_; }
    virtual double Qty() const { return qty_; };
    virtual uint64_t OrderId() const { return order_id_; };
    virtual aos::Side Side() const { return side_; };
    virtual aoe::bybit::TimeInForce TimeInForce() const {
        return time_in_force_;
    };
    virtual aoe::bybit::OrderRouting OrderRouting() const {
        return order_routing_;
    };

    virtual void SetTradingPair(common::TradingPair trading_pair) {
        trading_pair_ = trading_pair_;
    };
    virtual void SetCategory(aoe::bybit::Category category) {
        category_ = category;
    };
    virtual void SetPrice(double price) { price_ = price; }
    virtual void SetQty(double qty) { qty_ = qty; }
    virtual void SetOrderId(uint64_t order_id) { order_id = order_id_; };
    virtual void SetSide(aos::Side side) { side_ = side; };
    virtual void SetTimeInForce(aoe::bybit::TimeInForce time_in_force) {
        time_in_force_ = time_in_force;
    };
    virtual void SetOrderRouting(aoe::bybit::OrderRouting order_routing) {
        order_routing_ = order_routing;
    };

  protected:
    common::ExchangeId exchange_id_ = common::ExchangeId::kBybit;
    common::TradingPair trading_pair_;
    aoe::bybit::Category category_ = aoe::bybit::Category::kInvalid;
    double price_;
    double qty_;
    uint64_t order_id_;
    aos::Side side_                        = aos::Side::kInvalid;
    aoe::bybit::TimeInForce time_in_force_ = aoe::bybit::TimeInForce::kInvalid;
    aoe::bybit::OrderRouting order_routing_;
};
};  // namespace bybit
};  // namespace aoe