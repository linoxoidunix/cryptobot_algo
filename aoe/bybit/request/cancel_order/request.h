#pragma once
#include "aoe/bybit/enum_printer/enum_printer.h"
#include "aoe/bybit/request/cancel_order/i_request.h"
#include "aoe/bybit/request/place_order/request.h"
#include "aos/converters/trading_pair_to_big_string_view/trading_pair_to_big_string_view.h"
#include "aos/trading_pair_printer/i_trading_pair_printer.h"

namespace aoe {
namespace bybit {
namespace cancel_order {
namespace impl {

template <template <typename> typename MemoryPool>
class Spot : public RequestInterface<MemoryPool> {
  public:
    Spot() { this->SetCategory(aoe::bybit::Category::kSpot); }
    std::pair<bool, nlohmann::json> Accept(
        aoe::bybit::cancel_order::RequestMakerInterface<MemoryPool>*) override {
        nlohmann::json order;
        // set mandatory field
        {
            auto [status, value] = CategoryToString(this->Category());
            if (!status) return {false, {}};
            order["category"] = value;  // Значение категории
        }
        {
            auto [status, value] =
                aos::impl::TradingPairToBigStringView::Convert(
                    this->TradingPair());
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
class Linear : public RequestInterface<MemoryPool> {
    aos::impl::TradingPairToBigStringView trading_pair_printer_;

  public:
    Linear() { this->SetCategory(aoe::bybit::Category::kLinear); }
    std::pair<bool, nlohmann::json> Accept(
        aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*) override {
        nlohmann::json order;
        // set mandatory field
        {
            auto [status, value] = CategoryToString(this->Category());
            if (!status) return {false, {}};
            order["category"] = value;  // Значение категории
        }
        {
            auto [status, value] =
                aos::impl::TradingPairToBigStringView::Convert(
                    this->TradingPair());
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
    ~Linear() override = default;
};
};  // namespace impl
};  // namespace cancel_order
};  // namespace bybit
};  // namespace aoe