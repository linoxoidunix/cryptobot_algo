#pragma once
#include <memory>

#include "aos/position_strategy/i_position_strategy.h"
#include "boost/intrusive_ptr.hpp"

namespace aos {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class Position {
    Qty net_qty_{0};
    Price avg_price_{0};
    boost::intrusive_ptr<IPositionStrategy<Price, Qty, MemoryPool>> strategy_;

  public:
    Position(boost::intrusive_ptr<IPositionStrategy<Price, Qty, MemoryPool>>
                 strategy)
        : strategy_(strategy) {}

    // Добавить количество по заданной цене
    void AddPosition(Price price, Qty qty) {
        strategy_->Add(avg_price_, net_qty_, price, qty);
    }

    // Удалить количество по заданной цене
    void RemovePosition(Price price, Qty qty) {
        strategy_->Remove(avg_price_, net_qty_, price, qty);
    }

    // Получить чистую позицию
    Qty GetPosition() const { return net_qty_; }

    // Получить среднюю цену по чистой позиции
    Price GetAveragePrice() const { return avg_price_; }

    // Проверить, пуста ли позиция
    bool IsEmpty() const { return net_qty_ == 0; }
};

}  // namespace impl
}  // namespace aos