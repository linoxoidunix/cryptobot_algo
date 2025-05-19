#pragma once
#include "aoe/binance/enum_printer/enum_printer.h"
#include "aoe/binance/request/place_order/i_request.h"
#include "aos/converters/trading_pair_to_big_string_view/trading_pair_to_big_string_view.h"
namespace aoe {
namespace binance {
namespace place_order {

namespace impl {

template <template <typename> typename MemoryPool>
class Standart : public RequestInterface<MemoryPool> {
  public:
    ~Standart() override = default;
    std::pair<bool, nlohmann::json> Accept(
        aoe::binance::place_order::RequestMakerInterface<MemoryPool>*
            request_maker) override {
        return {};
    };
};

template <template <typename> typename MemoryPool>
class SpotBuyLimit : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    SpotBuyLimit() {
        this->SetOrderSide(aoe::binance::Side::kBuy);
        this->SetCategory(aoe::binance::Category::kSpot);
        this->SetOrderMode(aoe::binance::OrderMode::kLimit);
    }
    std::pair<bool, nlohmann::json> Accept(
        aoe::binance::place_order::RequestMakerInterface<MemoryPool>*
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
                trading_pair_printer_.Convert(this->TradingPair());
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
        order["qty"] =
            std::to_string(this->Qty());  // ToDo выравнивание количества
        order["price"] =
            std::to_string(this->Price());  // ToDo выравнивание цены
        // set not mandatory
        {
            auto [status, value] = TimeInForceToString(this->TimeInForce());
            if (status) order["timeInForce"] = value;  // Значение категории
        }
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~SpotBuyLimit() override = default;
};
template <template <typename> typename MemoryPool>
class SpotSellLimit : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    SpotSellLimit() {
        this->SetOrderSide(aos::Side::kSell);
        this->SetCategory(aoe::binance::Category::kSpot);
        this->SetOrderMode(aoe::binance::OrderMode::kLimit);
    }
    nlohmann::json Accept(
        aoe::binance::place_order::RequestMakerInterface<MemoryPool>*
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
                trading_pair_printer_.Convert(this->TradingPair());
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
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~SpotSellLimit() override = default;
};

template <template <typename> typename MemoryPool>
class SpotBuyMarket : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    SpotBuyMarket() {
        this->SetOrderSide(aoe::binance::Side::kBuy);
        this->SetCategory(aoe::binance::Category::kSpot);
        this->SetOrderMode(aoe::binance::OrderMode::kMarket);
    }
    nlohmann::json Accept(
        aoe::binance::place_order::RequestMakerInterface<MemoryPool>*
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
                trading_pair_printer_.Convert(this->TradingPair());
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
        order["qty"] = this->Qty();  // ToDo выравнивание количества
        // set not mandatory
        {
            auto [status, value] = TimeInForceToString(this->TimeInForce());
            if (status) order["timeInForce"] = value;  // Значение категории
        }
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~SpotBuyMarket() override = default;
};
template <template <typename> typename MemoryPool>
class SpotSellMarket : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    SpotSellMarket() {
        this->SetOrderSide(aos::Side::kSell);
        this->SetCategory(aoe::binance::Category::kSpot);
        this->SetOrderMode(aoe::binance::OrderMode::kMarket);
    }
    nlohmann::json Accept(
        aoe::binance::place_order::RequestMakerInterface<MemoryPool>*
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
                trading_pair_printer_.Convert(this->TradingPair());
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
        order["qty"] = this->Qty();  // ToDo выравнивание количества
        // set not mandatory
        {
            auto [status, value] = TimeInForceToString(this->TimeInForce());
            if (status) order["timeInForce"] = value;  // Значение категории
        }
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~SpotSellMarket() override = default;
};

template <template <typename> typename MemoryPool>
class FuturesBuyLimit : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    FuturesBuyLimit() {
        this->SetOrderSide(aoe::binance::Side::kBuy);
        this->SetCategory(aoe::binance::Category::kFutures);
        this->SetOrderMode(aoe::binance::OrderMode::kLimit);
    }
    nlohmann::json Accept(
        aoe::binance::place_order::RequestMakerInterface<MemoryPool>*
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
                trading_pair_printer_.Convert(this->TradingPair());
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
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~FuturesBuyLimit() override = default;
};
template <template <typename> typename MemoryPool>
class FuturesSellLimit : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    FuturesSellLimit() {
        this->SetOrderSide(aos::Side::kSell);
        this->SetCategory(aoe::binance::Category::kFutures);
        this->SetOrderMode(aoe::binance::OrderMode::kLimit);
    }
    nlohmann::json Accept(
        aoe::binance::place_order::RequestMakerInterface<MemoryPool>*
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
                trading_pair_printer_.Convert(this->TradingPair());
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
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~FuturesSellLimit() override = default;
};

template <template <typename> typename MemoryPool>
class FuturesBuyMarket : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    FuturesBuyMarket() {
        this->SetOrderSide(aoe::binance::Side::kBuy);
        this->SetCategory(aoe::binance::Category::kFutures);
        this->SetOrderMode(aoe::binance::OrderMode::kMarket);
    }
    nlohmann::json Accept(
        aoe::binance::place_order::RequestMakerInterface<MemoryPool>*
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
                trading_pair_printer_.Convert(this->TradingPair());
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
        order["qty"] = this->Qty();  // ToDo выравнивание количества
        // set not mandatory
        {
            auto [status, value] = TimeInForceToString(this->TimeInForce());
            if (status) order["timeInForce"] = value;  // Значение категории
        }
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~FuturesBuyMarket() override = default;
};
template <template <typename> typename MemoryPool>
class FuturesSellMarket : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    FuturesSellMarket() {
        this->SetOrderSide(aos::Side::kSell);
        this->SetCategory(aoe::binance::Category::kFutures);
        this->SetOrderMode(aoe::binance::OrderMode::kMarket);
    }
    nlohmann::json Accept(
        aoe::binance::place_order::RequestMakerInterface<MemoryPool>*
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
                trading_pair_printer_.Convert(this->TradingPair());
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
        order["qty"] = this->Qty();  // ToDo выравнивание количества
        // set not mandatory
        {
            auto [status, value] = TimeInForceToString(this->TimeInForce());
            if (status) order["timeInForce"] = value;  // Значение категории
        }
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~FuturesSellMarket() override = default;
};
};  // namespace impl
};  // namespace place_order
};  // namespace binance
};  // namespace aoe