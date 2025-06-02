#pragma once
#include <string_view>
namespace aoe {
namespace binance {
constexpr std::string_view kContentType = "application/x-www-form-urlencoded";
constexpr std::string_view kRESTPort    = "443";

namespace main_net {
namespace rest {

namespace spot {
// https://developers.binance.com/docs/binance-spot-api-docs/rest-api/general-api-information
constexpr std::string_view kRESTHost1 = "api.binance.com";
constexpr std::string_view kRESTHost2 = "api-gcp.binance.com";
};  // namespace spot
};  // namespace rest
};  // namespace main_net

namespace main_net {
namespace rest {
namespace futures {
// https://developers.binance.com/docs/binance-spot-api-docs/rest-api/general-api-information
constexpr std::string_view kRESTHost1 = "fapi.binance.com";
};  // namespace futures
};  // namespace rest
};  // namespace main_net

namespace main_net {
namespace ws {
namespace spot {
constexpr std::string_view kWSHost1 = "stream.binance.com";
};  // namespace spot
};  // namespace ws
};  // namespace main_net

namespace main_net {
namespace ws {
namespace futures {
constexpr std::string_view kWSHost1 = "fstream.binance.com";
};  // namespace futures
};  // namespace ws
};  // namespace main_net

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

/**
 * @enum OrderStatus
 * @brief Represents the status of an order in the trading engine.
 *
 * This enum defines the various stages an order may go through during its
 * lifecycle on an exchange, from creation to completion or cancellation.
 */
enum class OrderStatus {
    /**
     * @brief Invalid or uninitialized status.
     */
    kInvalid,

    // ===== Open statuses =====

    /**
     * @brief The order has been accepted by the matching engine and is active.
     */
    kNew,

    /**
     * @brief The order is in a pending state until the working order of an
     * order list is fully filled.
     */
    kPendingNew,

    /**
     * @brief A portion of the order has been filled; the remainder is still
     * active.
     */
    kPartiallyFilled,

    // ===== Closed statuses =====

    /**
     * @brief The order has been fully filled and completed.
     */
    kFilled,

    /**
     * @brief The order has been canceled by the user.
     */
    kCancelled,

    /**
     * @brief The order is in the process of being canceled. Currently unused.
     */
    kPendingCancel,

    /**
     * @brief The order was rejected and not processed by the matching engine.
     */
    kRejected,

    /**
     * @brief The order expired based on its type rules (e.g., LIMIT FOK with no
     * fill), or was canceled by the exchange (e.g., during liquidation or
     * maintenance).
     */
    kExpired,

    /**
     * @brief The order was expired by the exchange due to Self-Trade Prevention
     * (STP), such as when an EXPIRE_TAKER order would match against another
     * order from the same account or trade group.
     */
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

namespace futures {
// it is unused. used for partiatial depth
// https://developers.binance.com/docs/derivatives/usds-margined-futures/websocket-market-streams/Partial-Book-Depth-Streams
enum class Depth { k5, k10, k20 };
};  // namespace futures

namespace futures {
// https://developers.binance.com/docs/derivatives/usds-margined-futures/websocket-market-streams/Diff-Book-Depth-Streams
enum class DiffUpdateSpeed_ms { k100, k250, k500 };
};  // namespace futures

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