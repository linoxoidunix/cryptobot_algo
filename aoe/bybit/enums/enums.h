// Copyright 2025 Denis Evlanov

#pragma once
#include <functional>
#include <type_traits>

namespace aoe {
namespace bybit {
enum class ExecType { kTrade, kUnknown };  // NOLINT
enum class Category {                      // NOLINT
    kInvalid,
    kSpot,
    kLinear,
    kInverse,
    // kOption
};

using CategoryRaw = std::underlying_type_t<Category>;

enum class StopOrderType {  // NOLINT
    kInvalid,
    kTakeProfit,
    kStopLoss,
    kTrailingStop,
    kStop,
    kPartialTakeProfit,
    kPartialStopLoss
};

/**
 * @enum OrderStatus
 * @brief Enumeration of possible order statuses on an exchange.
 *
 * Represents the lifecycle stages of an order, including open, closed, and
 * conditional states.
 */
enum class OrderStatus {  // NOLINT
    /**
     * @brief Invalid or uninitialized order status.
     */
    kInvalid,

    // ===== Open statuses =====

    /**
     * @brief A newly created order that is active and placed in the order book.
     */
    kNew,

    /**
     * @brief An order that has been partially filled; the remaining quantity is
     * still active.
     */
    kPartiallyFilled,

    /**
     * @brief A conditional order that has been created but not yet triggered
     * (e.g., stop orders).
     */
    kUntriggered,

    // ===== Closed statuses =====

    /**
     * @brief The order was rejected (e.g., due to insufficient funds or invalid
     * parameters).
     */
    kRejected,

    /**
     * @brief The order was partially filled and then canceled.
     * Only applicable on the spot market.
     */
    kPartiallyFilledCanceled,

    /**
     * @brief The order was completely filled.
     */
    kFilled,

    /**
     * @brief The order was canceled.
     * On derivatives markets, canceled orders may still have some executed
     * quantity.
     */
    kCancelled,

    /**
     * @brief A conditional order was triggered and immediately transitioned
     * from `Untriggered` to `New`.
     */
    kTriggered,

    /**
     * @brief The order was deactivated before being triggered.
     * Used for take-profit/stop-loss, conditional, or OCO orders on the spot
     * market.
     */
    kDeactivated
};

enum class Side {  // NOLINT
    kInvalid,
    kSell,
    kBuy,
};

enum class OrderMode {  // NOLINT
    kInvalid,
    kMarket,
    kLimit,
};

enum class TimeInForce {  // NOLINT
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

enum class OrderRouting {  // NOLINT
    kInternal,             // Внутренний (например, обработка внутри системы)
    kExternal,             // Внешний (отправка на биржу)
    kSmart,    // Умный маршрут (используется для лучшего исполнения)
    kDarkPool  // Отправка в тёмный пул ликвидности
};

enum class PendingAction {  // NOLINT
    kNone,                  // нет активного действия
    kPlace,                 // ожидается подтверждение размещения
    kAmend,                 // ожидается подтверждение изменения
    kCancel,                // ожидается подтверждение отмены
    kTrigger                // (опционально) ожидается срабатывание стопа
};

namespace spot {
enum class Depth { k1, k50, k200 };  // NOLINT
};

namespace linear {
enum class Depth { k1, k50, k200, k500 };  // NOLINT
};

namespace inverse {
enum class Depth { k1, k50, k200, k500 };  // NOLINT
};

namespace option {
enum class Depth { k25, k100 };  // NOLINT
};
};  // namespace bybit
};  // namespace aoe

namespace std {
template <>
struct hash<aoe::bybit::Category> {
    std::underlying_type_t<aoe::bybit::Category> operator()(
        aoe::bybit::Category pair) const {
        return static_cast<std::underlying_type_t<aoe::bybit::Category>>(pair);
    }
};
};  // namespace std