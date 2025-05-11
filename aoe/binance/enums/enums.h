#pragma once
namespace aoe {
namespace binance {
enum class ExecType { kTrade, kUnknown };
enum class Category {
    kInvalid,
    kSpot,
    kFutures,  //?
    // kOption
};

// enum class OrderType {
//     kUnknown,
//     kMarket,
//     kLimit,
// };

// https://github.com/binance/binance-spot-api-docs/blob/master/enums.md
enum class StopOrderType {
    kInvalid,
    kStopLoss,
    kStopLossLimit,
    kTakeProfit,
    kTakeProfitLimit,
};

enum class OrderStatus {
    kInvalid,
    // open status below
    kNew,
    kPendingNew,
    kPartiallyFilled,
    // closed status below
    kFilled,
    kCancelled,
    kPendingCancel,
    kRejected,
    kExpired,
    kExpiredInMatch
};

enum class Side {
    kInvalid,
    kSell,
    kBuy,
};

enum class OrderMode {
    kInvalid,
    kMarket,
    kLimit,
};

enum class TimeInForce {
    kInvalid,
    kGTC,  // До отмены
    kIOC,  // Исполнить сразу или отменить
    kFOK,  // Полностью исполнить или отменить
};

enum class OrderRouting {
    kInternal,  // Внутренний (например, обработка внутри системы)
    kExternal,  // Внешний (отправка на биржу)
    kSmart,     // Умный маршрут (используется для лучшего исполнения)
    kDarkPool   // Отправка в тёмный пул ликвидности
};

enum class PendingAction {
    kNone,    // нет активного действия
    kPlace,   // ожидается подтверждение размещения
    kAmend,   // ожидается подтверждение изменения
    kCancel,  // ожидается подтверждение отмены
    kTrigger  // (опционально) ожидается срабатывание стопа
};

namespace spot {
enum class Depth { k1, k5, k10, k20 };
};

namespace spot {
enum class DiffUpdateSpeed_ms { k100, k1000 };
};

// namespace linear {
// enum class Depth { k1, k50, k200, k500 };
// };

// namespace inverse {
// enum class Depth { k1, k50, k200, k500 };
// };

// namespace option {
// enum class Depth { k25, k100 };
// };
};  // namespace binance
};  // namespace aoe