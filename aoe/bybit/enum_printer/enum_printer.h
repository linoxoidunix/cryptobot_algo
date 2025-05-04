#pragma once
#include <string_view>

#include "aoe/bybit/enums/enums.h"
namespace aoe {
namespace bybit {
// Функция для печати значений ExecType
struct EnumValueWithStatus {
    bool status;
    std::string_view value;
};
EnumValueWithStatus ExecTypeToString(ExecType execType) {
    switch (execType) {
        case ExecType::kTrade:
            return {true, "Trade"};
        case ExecType::kUnknown:
            return {false, {}};
        default:
            return {false, {}};  // -1 — неизвестная ошибка
    }
};

EnumValueWithStatus CategoryToString(Category category) {
    switch (category) {
        case Category::kInvalid:
            return {false, {}};
        case Category::kSpot:
            return {true, "spot"};
        case Category::kLinear:
            return {true, "linear"};
        case Category::kInverse:
            return {true, "inverse"};
        default:
            return {false, ""};
    }
}

EnumValueWithStatus StopOrderTypeToString(StopOrderType stopOrderType) {
    switch (stopOrderType) {
        case StopOrderType::kInvalid:
            return {false, {}};
        case StopOrderType::kTakeProfit:
            return {true, "TakeProfit"};
        case StopOrderType::kStopLoss:
            return {true, "StopLoss"};
        case StopOrderType::kTrailingStop:
            return {true, "TrailingStop"};
        case StopOrderType::kStop:
            return {true, "Stop"};
        case StopOrderType::kPartialTakeProfit:
            return {true, "PartialTakeProfit"};
        case StopOrderType::kPartialStopLoss:
            return {true, "PartialStopLoss"};
        default:
            return {false, {}};
    }
}

EnumValueWithStatus OrderStatusToString(OrderStatus orderStatus) {
    switch (orderStatus) {
        case OrderStatus::kInvalid:
            return {false, {}};
        case OrderStatus::kNew:
            return {true, "New"};
        case OrderStatus::kPartiallyFilled:
            return {true, "PartiallyFilled"};
        case OrderStatus::kUntriggered:
            return {true, "Untriggered"};
        case OrderStatus::kRejected:
            return {false, "Rejected"};
        case OrderStatus::kPartiallyFilledCanceled:
            return {false, "PartiallyFilledCanceled"};
        case OrderStatus::kFilled:
            return {true, "Filled"};
        case OrderStatus::kCancelled:
            return {false, "Cancelled"};
        case OrderStatus::kTriggered:
            return {true, "Triggered"};
        case OrderStatus::kDeactivated:
            return {false, "Deactivated"};
        default:
            return {false, ""};
    }
}

EnumValueWithStatus SideToString(Side side) {
    switch (side) {
        case Side::kInvalid:
            return {false, {}};
        case Side::kSell:
            return {true, "Sell"};
        case Side::kBuy:
            return {true, "Buy"};
        default:
            return {false, ""};
    }
}

EnumValueWithStatus OrderModeToString(OrderMode orderMode) {
    switch (orderMode) {
        case OrderMode::kInvalid:
            return {false, {}};
        case OrderMode::kMarket:
            return {true, "Market"};
        case OrderMode::kLimit:
            return {true, "Limit"};
        default:
            return {false, {}};
    }
}

EnumValueWithStatus TimeInForceToString(TimeInForce timeInForce) {
    switch (timeInForce) {
        case TimeInForce::kInvalid:
            return {false, {}};
        case TimeInForce::kGTC:
            return {true, "GTC"};
        case TimeInForce::kIOC:
            return {true, "IOC"};
        case TimeInForce::kFOK:
            return {true, "FOK"};
        case TimeInForce::kDAY:
            return {true, "DAY"};
        case TimeInForce::kGTD:
            return {true, "GTD"};
        case TimeInForce::kOPG:
            return {true, "OPG"};
        case TimeInForce::kCLS:
            return {true, "CLS"};
        case TimeInForce::kGTX:
            return {true, "GTX"};
        case TimeInForce::kMOC:
            return {true, "MOC"};
        case TimeInForce::kLOC:
            return {true, "LOC"};
        default:
            return {false, {}};
    }
}

EnumValueWithStatus OrderRoutingToString(OrderRouting orderRouting) {
    switch (orderRouting) {
        case OrderRouting::kInternal:
            return {true, "Internal"};
        case OrderRouting::kExternal:
            return {true, "External"};
        case OrderRouting::kSmart:
            return {true, "Smart"};
        case OrderRouting::kDarkPool:
            return {true, "DarkPool"};
        default:
            return {false, ""};
    }
}

namespace spot {
EnumValueWithStatus DepthToString(Depth depth) {
    switch (depth) {
        case Depth::k1:
            return {true, "1"};
        case Depth::k50:
            return {true, "50"};
        case Depth::k200:
            return {true, "200"};
        default:
            return {false, ""};
    }
};
};  // namespace spot

namespace linear {
EnumValueWithStatus DepthToString(Depth depth) {
    switch (depth) {
        case Depth::k1:
            return {true, "1"};
        case Depth::k50:
            return {true, "50"};
        case Depth::k200:
            return {true, "200"};
        case Depth::k500:
            return {true, "500"};
        default:
            return {false, ""};
    }
};
};  // namespace linear

namespace inverse {
EnumValueWithStatus DepthToString(Depth depth) {
    switch (depth) {
        case Depth::k1:
            return {true, "1"};
        case Depth::k50:
            return {true, "50"};
        case Depth::k200:
            return {true, "200"};
        case Depth::k500:
            return {true, "500"};
        default:
            return {false, ""};
    }
};
};  // namespace inverse

namespace option {
EnumValueWithStatus DepthToString(Depth depth) {
    switch (depth) {
        case Depth::k25:
            return {true, "25"};
        case Depth::k100:
            return {true, "100"};
        default:
            return {false, ""};
    }
};
};  // namespace option
};  // namespace bybit
};  // namespace aoe