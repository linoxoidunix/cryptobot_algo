#pragma once
#include "aoe/bybit/enum_printer/enum_printer.h"
#include "aoe/bybit/request/place_order/i_order_type.h"
#include "aos/trading_pair_printer/i_trading_pair_printer.h"
namespace aoe {
namespace bybit {
namespace impl {

template <template <typename> typename MemoryPool>
class OrderDefault : public OrderTypeInterface<MemoryPool> {
  public:
    ~OrderDefault() override = default;
    std::pair<bool, nlohmann::json> Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) override {
        return {};
    };
};

template <template <typename> typename MemoryPool>
class SpotBuyLimitOrder : public OrderTypeInterface<MemoryPool> {
    aos::TradingPairPrinterInterface& trading_pair_printer_;

  public:
    SpotBuyLimitOrder(aos::TradingPairPrinterInterface& trading_pair_printer)
        : trading_pair_printer_(trading_pair_printer) {
        this->SetOrderSide(aoe::bybit::Side::kBuy);
        this->SetCategory(aoe::bybit::Category::kSpot);
        this->SetOrderMode(aoe::bybit::OrderMode::kLimit);
    }
    std::pair<bool, nlohmann::json> Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) override {
        nlohmann::json order;
        // set mandatory field
        {
            auto [status, value] = CategoryToString(this->Category());
            if (!status) return {false, {}};
            order["category"] = value;  // Значение категории
        }
        {
            auto [status, value] =
                trading_pair_printer_.Print(this->TradingPair());
            if (!status) return {false, {}};
            order["symbol"] = value;  // Значение категории
        }
        {
            auto [status, value] = SideToString(this->OrderSide());
            if (!status) return {false, {}};
            order["side"] = value;  // Значение категории
        }
        {
            auto [status, value] = OrderModeToString(this->OrderMode());
            if (!status) return {false, {}};
            order["orderType"] = value;  // Значение категории
        }
        order["qty"]   = this->Qty();    // ToDo выравнивание количества
        order["price"] = this->Price();  // ToDo выравнивание цены
        // set not mandatory
        {
            auto [status, value] = TimeInForceToString(this->TimeInForce());
            if (status) order["timeInForce"] = value;  // Значение категории
        }
        return {true, order};
    };
    ~SpotBuyLimitOrder() override = default;
};
template <template <typename> typename MemoryPool>
class SpotSellLimitOrder : public OrderTypeInterface<MemoryPool> {
  public:
    SpotSellLimitOrder() {
        this->SetOrderSide(aos::Side::kSell);
        this->SetCategory(aoe::bybit::Category::kSpot);
        this->SetOrderMode(aoe::bybit::OrderMode::kLimit);
    }
    nlohmann::json Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) override {
        return {};
    };
    ~SpotSellLimitOrder() override = default;
};

template <template <typename> typename MemoryPool>
class SpotBuyMarketOrder : public OrderTypeInterface<MemoryPool> {
  public:
    SpotBuyMarketOrder() {
        this->SetOrderSide(aoe::bybit::Side::kBuy);
        this->SetCategory(aoe::bybit::Category::kSpot);
        this->SetOrderMode(aoe::bybit::OrderMode::kMarket);
    }
    nlohmann::json Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) override {
        return {};
    };
    ~SpotBuyMarketOrder() override = default;
};
template <template <typename> typename MemoryPool>
class SpotSellMarketOrder : public OrderTypeInterface<MemoryPool> {
  public:
    SpotSellMarketOrder() {
        this->SetOrderSide(aos::Side::kSell);
        this->SetCategory(aoe::bybit::Category::kSpot);
        this->SetOrderMode(aoe::bybit::OrderMode::kMarket);
    }
    nlohmann::json Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) override {
        return {};
    };
    ~SpotSellMarketOrder() override = default;
};

template <template <typename> typename MemoryPool>
class LinearBuyLimitOrder : public OrderTypeInterface<MemoryPool> {
  public:
    LinearBuyLimitOrder() {
        this->SetOrderSide(aoe::bybit::Side::kBuy);
        this->SetCategory(aoe::bybit::Category::kLinear);
        this->SetOrderMode(aoe::bybit::OrderMode::kLimit);
    }
    nlohmann::json Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) override {
        return {};
    };
    ~LinearBuyLimitOrder() override = default;
};
template <template <typename> typename MemoryPool>
class LinearSellLimitOrder : public OrderTypeInterface<MemoryPool> {
  public:
    LinearSellLimitOrder() {
        this->SetOrderSide(aos::Side::kSell);
        this->SetCategory(aoe::bybit::Category::kLinear);
        this->SetOrderMode(aoe::bybit::OrderMode::kLimit);
    }
    nlohmann::json Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) override {
        return {};
    };
    ~LinearSellLimitOrder() override = default;
};

template <template <typename> typename MemoryPool>
class LinearBuyMarketOrder : public OrderTypeInterface<MemoryPool> {
  public:
    LinearBuyMarketOrder() {
        this->SetOrderSide(aoe::bybit::Side::kBuy);
        this->SetCategory(aoe::bybit::Category::kLinear);
        this->SetOrderMode(aoe::bybit::OrderMode::kMarket);
    }
    nlohmann::json Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) override {
        return {};
    };
    ~LinearBuyMarketOrder() override = default;
};
template <template <typename> typename MemoryPool>
class LinearSellMarketOrder : public OrderTypeInterface<MemoryPool> {
  public:
    LinearSellMarketOrder() {
        this->SetOrderSide(aos::Side::kSell);
        this->SetCategory(aoe::bybit::Category::kLinear);
        this->SetOrderMode(aoe::bybit::OrderMode::kMarket);
    }
    nlohmann::json Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) override {
        return {};
    };
    ~LinearSellMarketOrder() override = default;
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe