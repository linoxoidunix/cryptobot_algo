#pragma once
#include <memory>

#include "aos/pnl/i_pnl.h"
#include "aos/position_strategy/i_position_strategy.h"
#include "aos/trading_pair/trading_pair.h"
#include "boost/intrusive_ptr.hpp"

namespace aos {
namespace impl {

template <typename Price, typename Qty>
class NetPositionDefault {
    Qty net_qty_{0};
    Price avg_price_{0};
    NetPositionStrategyInterface<Price, Qty>& strategy_;

  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    NetPositionDefault(NetPositionStrategyInterface<Price, Qty>& strategy)
        : strategy_(strategy) {}

    // Добавить количество по заданной цене
    void AddPosition(common::ExchangeId exchange_id,
                     aos::TradingPair trading_pair, Price price, Qty qty) {
        strategy_.Add(exchange_id, trading_pair, avg_price_, net_qty_, price,
                      qty);
    }

    // Удалить количество по заданной цене
    void RemovePosition(common::ExchangeId exchange_id,
                        aos::TradingPair trading_pair, Price price, Qty qty) {
        strategy_.Remove(exchange_id, trading_pair, avg_price_, net_qty_, price,
                         qty);
    }

    // Получить чистую позицию
    Qty GetPosition() const { return net_qty_; }

    // Получить среднюю цену по чистой позиции
    Price GetAveragePrice() const { return avg_price_; }

    // Проверить, пуста ли позиция
    bool IsEmpty() const { return net_qty_ == 0; }
};

template <typename Price, typename Qty>
class HedgedPositionDefault {
    Qty net_qty_[2]{0, 0};  // [0] - short, [1] - long
    Price avg_price_[2]{0, 0};
    HedgePositionStrategyInterface<Price, Qty>& strategy_;

  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    HedgedPositionDefault(HedgePositionStrategyInterface<Price, Qty>& strategy)
        : strategy_(strategy) {}

    // Добавить количество по заданной цене
    void AddPosition(common::ExchangeId exchange_id,
                     aos::TradingPair trading_pair, Price price, Qty qty) {
        strategy_.Add(exchange_id, trading_pair, avg_price_, net_qty_, price,
                      qty);
    }

    // Удалить количество по заданной цене
    void RemovePosition(common::ExchangeId exchange_id,
                        aos::TradingPair trading_pair, Price price, Qty qty) {
        strategy_.Remove(exchange_id, trading_pair, avg_price_, net_qty_, price,
                         qty);
    }

    // // Получить чистую позицию
    Qty GetPosition() const {
        return net_qty_[0] + net_qty_[1];  // возвращает сумму по всем позициям
    }

    bool IsEmpty() const { return net_qty_[0] == 0 && net_qty_[1] == 0; }

    Price GetAveragePrice() const {
        // Вычисление средней цены на основе количества и средней цены
        Price total_value =
            net_qty_[0] * avg_price_[0] + net_qty_[1] * avg_price_[1];
        Qty total_qty = net_qty_[0] + net_qty_[1];
        return total_qty != 0 ? total_value / total_qty : 0;
    }

    // // Получить лонг позицию
    Qty GetLongPosition() const {
        return net_qty_[1];  // возвращает сумму по всем позициям
    }

    Price GetAverageLongPrice() const {
        // Вычисление средней цены на основе количества и средней цены
        return avg_price_[1];
    }

    Price GetAverageShortPrice() const {
        // Вычисление средней цены на основе количества и средней цены
        return avg_price_[0];
    }

    // // Получить лонг позицию
    Qty GetShortPosition() const {
        return net_qty_[0];  // возвращает сумму по всем позициям
    }
};
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class NetPosition {
    Qty net_qty_{0};
    Price avg_price_{0};
    boost::intrusive_ptr<INetPositionStrategy<Price, Qty, MemoryPool>>
        strategy_;

  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    NetPosition(
        boost::intrusive_ptr<INetPositionStrategy<Price, Qty, MemoryPool>>
            strategy)
        : strategy_(strategy) {}

    // Добавить количество по заданной цене
    void AddPosition(common::ExchangeId exchange_id,
                     aos::TradingPair trading_pair, Price price, Qty qty) {
        strategy_->Add(exchange_id, trading_pair, avg_price_, net_qty_, price,
                       qty);
    }

    // Удалить количество по заданной цене
    void RemovePosition(common::ExchangeId exchange_id,
                        aos::TradingPair trading_pair, Price price, Qty qty) {
        strategy_->Remove(exchange_id, trading_pair, avg_price_, net_qty_,
                          price, qty);
    }

    // Получить чистую позицию
    Qty GetPosition() const { return net_qty_; }

    // Получить среднюю цену по чистой позиции
    Price GetAveragePrice() const { return avg_price_; }

    // Проверить, пуста ли позиция
    bool IsEmpty() const { return net_qty_ == 0; }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class HedgePosition {
    Qty net_qty_[2]{0, 0};  // [0] - short, [1] - long
    Price avg_price_[2]{0, 0};
    boost::intrusive_ptr<IHedgePositionStrategy<Price, Qty, MemoryPool>>
        strategy_;

  public:
    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    HedgePosition(
        boost::intrusive_ptr<IHedgePositionStrategy<Price, Qty, MemoryPool>>
            strategy)
        : strategy_(strategy) {}

    // Добавить количество по заданной цене
    void AddPosition(common::ExchangeId exchange_id,
                     aos::TradingPair trading_pair, Price price, Qty qty) {
        strategy_->Add(exchange_id, trading_pair, avg_price_, net_qty_, price,
                       qty);
    }

    // Удалить количество по заданной цене
    void RemovePosition(common::ExchangeId exchange_id,
                        aos::TradingPair trading_pair, Price price, Qty qty) {
        strategy_->Remove(exchange_id, trading_pair, avg_price_, net_qty_,
                          price, qty);
    }

    // // Получить чистую позицию
    Qty GetPosition() const {
        return net_qty_[0] + net_qty_[1];  // возвращает сумму по всем позициям
    }

    bool IsEmpty() const { return net_qty_[0] == 0 && net_qty_[1] == 0; }

    Price GetAveragePrice() const {
        // Вычисление средней цены на основе количества и средней цены
        Price total_value =
            net_qty_[0] * avg_price_[0] + net_qty_[1] * avg_price_[1];
        Qty total_qty = net_qty_[0] + net_qty_[1];
        return total_qty != 0 ? total_value / total_qty : 0;
    }
};

}  // namespace impl
}  // namespace aos