#pragma once
#include "aoe/binance/enum_printer/enum_printer.h"
#include "aoe/binance/request/cancel_order/i_request.h"
#include "aos/converters/trading_pair_to_big_string_view/trading_pair_to_big_string_view.h"
#include "aos/trading_pair_printer/i_trading_pair_printer.h"
namespace aoe {
namespace binance {
namespace cancel_order {
namespace impl {

template <template <typename> typename MemoryPool>
class Spot : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    Spot() { this->SetCategory(aoe::binance::Category::kSpot); }
    std::pair<bool, nlohmann::json> Accept(
        aoe::binance::cancel_order::RequestMakerInterface<MemoryPool>*
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
        // set not mandatory
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~Spot() override = default;
};
template <template <typename> typename MemoryPool>
class Futures : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    Futures() { this->SetCategory(aoe::binance::Category::kFutures); }
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
        // set not mandatory
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~Futures() override = default;
};
};  // namespace impl
};  // namespace cancel_order
};  // namespace binance
};  // namespace aoe