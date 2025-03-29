#pragma once
#include "aot/common/types.h"
#include "magic_enum/magic_enum.hpp"

namespace aos {
enum class OrderStatus {
    New,                 // Новый ордер
    PartiallyFilled,     // Частично исполнен
    Filled,              // Полностью исполнен
    CanceledByUser,      // Отменён пользователем
    CanceledByExchange,  // Отменён биржой
    Rejected,            // Отклонён биржей
    Expired,             // Истёк срок действия
    PendingCancel,       // В процессе отмены
    PendingNew,          // В процессе создания
    PendingReplace       // В процессе изменения
};
enum class TimeInForce {
    GTC,  // До отмены
    IOC,  // Исполнить сразу или отменить
    FOK,  // Полностью исполнить или отменить
    DAY,  // До конца дня
    GTD,  // До указанной даты
    OPG,  // Только на открытии
    CLS,  // Только на закрытии
    GTX,  // До кросс-торгов
    MOC,  // Маркет-ордер на закрытии
    LOC   // Лимит-ордер на закрытии
};

enum class OrderRouting {
    Internal,  // Внутренний (например, обработка внутри системы)
    External,  // Внешний (отправка на биржу)
    Smart,     // Умный маршрут (используется для лучшего исполнения)
    DarkPool   // Отправка в тёмный пул ликвидности
};

template <typename Price, typename Qty>
struct OrderData {
    common::ExchangeId exchange_id;
    common::TradingPair trading_pair;
    Price price;
    Qty qty;
    common::Side side;
    TimeInForce tif;
    OrderRouting routing;
    OrderStatus status;
};
};  // namespace aos

template <>
class fmt::formatter<aos::OrderStatus> {
  public:
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template <typename Context>
    constexpr auto format(const aos::OrderStatus& foo, Context& ctx) const {
        return fmt::format_to(ctx.out(), "OrderStatus:{}",
                              magic_enum::enum_name(foo));
    }
};