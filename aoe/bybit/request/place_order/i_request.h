#pragma once
#include "aoe/bybit/enums/enums.h"
#include "aoe/bybit/request_maker/for_web_socket/place_order/i_request_maker.h"
#include "aos/common/common.h"
#include "aos/common/exchange_id.h"
#include "aos/request/i_request.h"
#include "aos/trading_pair/trading_pair.h"

namespace aoe {
namespace bybit {
namespace place_order {
template <template <typename> typename MemoryPool>
class RequestInterface : public aos::RequestInterface<MemoryPool> {
  public:
    ~RequestInterface() override = default;
    virtual common::ExchangeId ExchangeId() const { return exchange_id_; };
    virtual const aos::TradingPair& TradingPair() const {
        return trading_pair_;
    };
    virtual aoe::bybit::Category Category() const { return category_; };
    virtual double Price() const { return price_; }
    virtual double Qty() const { return qty_; };
    virtual uint64_t OrderId() const { return order_id_; };
    virtual Side OrderSide() const { return side_; };
    virtual aoe::bybit::TimeInForce TimeInForce() const {
        return time_in_force_;
    };
    virtual aoe::bybit::OrderRouting OrderRouting() const {
        return order_routing_;
    };
    virtual aoe::bybit::OrderMode OrderMode() const { return order_mode_; };

    virtual void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    };
    virtual void SetCategory(aoe::bybit::Category category) {
        category_ = category;
    };
    virtual void SetPrice(double price) { price_ = price; }
    virtual void SetQty(double qty) { qty_ = qty; }
    virtual void SetOrderId(uint64_t order_id) { order_id_ = order_id; };
    virtual void SetOrderSide(Side side) { side_ = side; };
    virtual void SetTimeInForce(aoe::bybit::TimeInForce time_in_force) {
        time_in_force_ = time_in_force;
    };
    virtual void SetOrderRouting(aoe::bybit::OrderRouting order_routing) {
        order_routing_ = order_routing;
    };
    virtual void SetOrderMode(aoe::bybit::OrderMode order_mode) {
        order_mode_ = order_mode;
    };
    // json appearance for bybit
    virtual std::pair<bool, nlohmann::json> Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) = 0;

  protected:
    common::ExchangeId exchange_id_ = common::ExchangeId::kBybit;
    aos::TradingPair trading_pair_;
    aoe::bybit::Category category_ = aoe::bybit::Category::kInvalid;
    double price_                  = 0;
    double qty_                    = 0;
    uint64_t order_id_;
    Side side_                             = aoe::bybit::Side::kInvalid;
    aoe::bybit::TimeInForce time_in_force_ = aoe::bybit::TimeInForce::kInvalid;
    aoe::bybit::OrderRouting order_routing_;
    aoe::bybit::OrderMode order_mode_ = aoe::bybit::OrderMode::kInvalid;
};
};  // namespace place_order
};  // namespace bybit
};  // namespace aoe