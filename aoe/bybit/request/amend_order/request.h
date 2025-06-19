#pragma once
#include "aoe/bybit/enum_printer/enum_printer.h"
#include "aoe/bybit/request/amend_order/i_request.h"
#include "aos/trading_pair_printer/i_trading_pair_printer.h"
namespace aoe {
namespace bybit {
namespace amend_order {
namespace impl {

template <template <typename> typename MemoryPool>
class Spot : public RequestInterface<MemoryPool> {
    aos::TradingPairPrinterInterface& trading_pair_printer_;

  public:
    explicit Spot(aos::TradingPairPrinterInterface& trading_pair_printer)
        : trading_pair_printer_(trading_pair_printer) {
        this->SetCategory(aoe::bybit::Category::kSpot);
    }
    std::pair<bool, nlohmann::json> Accept(
        aoe::bybit::amend_order::RequestMakerInterface<MemoryPool>*) override {
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
        // set not mandatory
        {
            auto value           = (this->OrderId());
            order["orderLinkId"] = std::to_string(value);  // Значение категории
        }
        return {true, order};
    };
    ~Spot() override = default;
};
};  // namespace impl
};  // namespace amend_order
};  // namespace bybit
};  // namespace aoe