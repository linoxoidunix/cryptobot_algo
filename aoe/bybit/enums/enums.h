#pragma once
namespace aoe {
namespace bybit {
enum class ExecType { kTrade, kUnknown };
enum class Category {
    kInvalid,
    kSpot,
    kLinear,
    kInverse,
    // kOption
};

// enum class OrderType {
//     kUnknown,
//     kMarket,
//     kLimit,
// };

enum class StopOrderType {
    kInvalid,
    kTakeProfit,
    kStopLoss,
    kTrailingStop,
    kStop,
    kPartialTakeProfit,
    kPartialStopLoss
};

enum class OrderStatus {
    kInvalid,
    // open status below
    kNew,
    kPartiallyFilled,
    kUntriggered,
    // closed status below
    kRejected,
    kPartiallyFilledCanceled,
    kFilled,
    kCancelled,
    kTriggered,
    kDeactivated
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
    kDAY,  // До конца дня
    kGTD,  // До указанной даты
    kOPG,  // Только на открытии
    kCLS,  // Только на закрытии
    kGTX,  // До кросс-торгов
    kMOC,  // Маркет-ордер на закрытии
    kLOC   // Лимит-ордер на закрытии
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
enum class Depth { k1, k50, k200 };
};

namespace linear {
enum class Depth { k1, k50, k200, k500 };
};

namespace inverse {
enum class Depth { k1, k50, k200, k500 };
};

namespace option {
enum class Depth { k25, k100 };
};
};  // namespace bybit
};  // namespace aoe