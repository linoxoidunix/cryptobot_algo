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

inline EnumValueWithStatus ExecTypeToString(ExecType execType) {
    switch (execType) {
        case ExecType::kTrade:
            return {true, "TRADE"};
        default:
            return {false, {}};  // -1 — неизвестная ошибка
    }
    return {false, {}};
};

inline EnumValueWithStatus CategoryToString(Category category) {
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
    return {false, ""};
}

inline EnumValueWithStatus StopOrderTypeToString(StopOrderType stopOrderType) {
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

inline EnumValueWithStatus OrderStatusToString(OrderStatus orderStatus) {
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

inline EnumValueWithStatus SideToString(Side side) {
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

inline EnumValueWithStatus OrderModeToString(OrderMode orderMode) {
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

inline EnumValueWithStatus TimeInForceToString(TimeInForce timeInForce) {
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

inline EnumValueWithStatus OrderRoutingToString(OrderRouting orderRouting) {
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
inline EnumValueWithStatus DepthToString(Depth depth) {
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
inline EnumValueWithStatus DiffUpdateSpeed_ms_ToString(
    DiffUpdateSpeed_ms speed) {
    switch (speed) {
        case DiffUpdateSpeed_ms::k100:
            return {true, "100ms"};
        case DiffUpdateSpeed_ms::k1000:
            return {true, "1000ms"};
        default:
            return {false, ""};
    }
};
};  // namespace spot

namespace futures {
inline EnumValueWithStatus DiffUpdateSpeed_ms_ToString(
    DiffUpdateSpeed_ms speed) {
    switch (speed) {
        case DiffUpdateSpeed_ms::k100:
            return {true, "100ms"};
        case DiffUpdateSpeed_ms::k250:
            return {true, "250ms"};
        case DiffUpdateSpeed_ms::k500:
            return {true, "500ms"};
        default:
            return {false, ""};
    }
};
};  // namespace futures

};  // namespace binance
};  // namespace aoe