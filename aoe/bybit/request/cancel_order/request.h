#pragma once
#include "aoe/bybit/enum_printer/enum_printer.h"
#include "aoe/bybit/request/place_order/i_request.h"
#include "aos/trading_pair_printer/i_trading_pair_printer.h"
namespace aoe {
namespace bybit {
namespace cancel_order {
namespace impl {

template <template <typename> typename MemoryPool>
class CancelSpotOrder : public OrderTypeInterface<MemoryPool> {
    aos::TradingPairPrinterInterface& trading_pair_printer_;

  public:
    CancelSpotOrder(aos::TradingPairPrinterInterface& trading_pair_printer)
        : trading_pair_printer_(trading_pair_printer) {
        this->SetCategory(aoe::bybit::Category::kSpot);
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
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~CancelSpotOrder() override = default;
};
};  // namespace impl
};  // namespace cancel_order
};  // namespace bybit
};  // namespace aoe