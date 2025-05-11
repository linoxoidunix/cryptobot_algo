#pragma once
#include <string_view>

#include "aoe/binance/enums/enums.h"
namespace aoe {
namespace binance {
// Функция для печати значений ExecType
struct EnumValueWithStatus {
    bool status;
    std::string_view value;
};
EnumValueWithStatus ExecTypeToString(ExecType execType) {
    switch (execType) {
        case ExecType::kTrade:
            return {true, "TRADE"};
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
        case Category::kFutures:
            return {true, "linear"};
        default:
            return {false, ""};
    }
}

EnumValueWithStatus StopOrderTypeToString(StopOrderType stopOrderType) {
    switch (stopOrderType) {
        case StopOrderType::kInvalid:
            return {false, {}};
        case StopOrderType::kTakeProfit:
            return {true, "TAKE_PROFIT"};
        case StopOrderType::kStopLoss:
            return {true, "STOP_LOSS"};
        case StopOrderType::kStopLossLimit:
            return {true, "STOP_LOSS_LIMIT"};
        case StopOrderType::kTakeProfitLimit:
            return {true, "TAKE_PROFIT_LIMIT"};
        default:
            return {false, {}};
    }
}

EnumValueWithStatus OrderStatusToString(OrderStatus orderStatus) {
    switch (orderStatus) {
        case OrderStatus::kInvalid:
            return {false, {}};
        case OrderStatus::kNew:
            return {true, "NEW"};
        case OrderStatus::kPendingNew:
            return {true, "PENDING_NEW"};
        case OrderStatus::kPartiallyFilled:
            return {true, "PARTIALLY_FILLED"};
        case OrderStatus::kFilled:
            return {false, "FILLED"};
        case OrderStatus::kCancelled:
            return {false, "CANCELED"};
        case OrderStatus::kPendingCancel:
            return {true, "PENDING_CANCEL"};
        case OrderStatus::kRejected:
            return {false, "REJECTED"};
        case OrderStatus::kExpired:
            return {true, "EXPIRED"};
        case OrderStatus::kExpiredInMatch:
            return {false, "EXPIRED_IN_MATCH"};
        default:
            return {false, ""};
    }
}

EnumValueWithStatus SideToString(Side side) {
    switch (side) {
        case Side::kInvalid:
            return {false, {}};
        case Side::kSell:
            return {true, "SELL"};
        case Side::kBuy:
            return {true, "BUY"};
        default:
            return {false, ""};
    }
}

EnumValueWithStatus OrderModeToString(OrderMode orderMode) {
    switch (orderMode) {
        case OrderMode::kInvalid:
            return {false, {}};
        case OrderMode::kMarket:
            return {true, "MARKET"};
        case OrderMode::kLimit:
            return {true, "LIMIT"};
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
        case Depth::k5:
            return {true, "5"};
        case Depth::k10:
            return {true, "10"};
        case Depth::k20:
            return {true, "20"};
        default:
            return {false, ""};
    }
};
};  // namespace spot
namespace spot {
EnumValueWithStatus DiffUpdateSpeed_ms_ToString(DiffUpdateSpeed_ms speed) {
    switch (speed) {
        case DiffUpdateSpeed_ms::k100:
            return {true, "100"};
        case DiffUpdateSpeed_ms::k1000:
            return {true, "1000"};
        default:
            return {false, ""};
    }
};
};  // namespace spot

};  // namespace binance
};  // namespace aoe