#pragma once
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>

#include "aoe/binance/order_storage/i_order_storage.h"
namespace aoe {
namespace binance {
namespace impl {
using namespace boost::multi_index;

struct ByOrderId {};
struct ByStatus {};
struct ByCategory {};
struct ByCategorySide {};
struct ByCategoryMode {};
struct ByCategorySideMode {};

using OrderSet = multi_index_container<
    Order,
    indexed_by<
        ordered_unique<tag<ByOrderId>,
                       member<Order, uint64_t, &Order::order_id> >,
        ordered_non_unique<tag<ByStatus>,
                           member<Order, OrderStatus, &Order::order_status> >,
        ordered_non_unique<tag<ByCategory>,
                           member<Order, Category, &Order::category> >,
        ordered_non_unique<
            tag<ByCategorySide>,
            composite_key<Order, member<Order, Category, &Order::category>,
                          member<Order, Side, &Order::side> > >,
        ordered_non_unique<
            tag<ByCategoryMode>,
            composite_key<Order, member<Order, Category, &Order::category>,
                          member<Order, OrderMode, &Order::order_mode> > > > >;

class OrderStorage : public OrderStorageInterface {
  public:
    ~OrderStorage() override = default;
    std::vector<const Order*> Get(Category category) override {
        std::vector<const Order*> result;
        const auto& index  = orders_.get<ByCategory>();
        auto [first, last] = index.equal_range(category);
        result.reserve(
            std::distance(first, last));  // ← резервируем нужное количество
        for (auto it = first; it != last; ++it) result.push_back(&*it);
        return result;
    }

    std::vector<const Order*> Get(Category category, Side side) override {
        std::vector<const Order*> result;
        const auto& index  = orders_.get<ByCategorySide>();
        auto [first, last] = index.equal_range(std::make_tuple(category, side));
        result.reserve(
            std::distance(first, last));  // ← резервируем нужное количество
        for (auto it = first; it != last; ++it) result.push_back(&*it);
        return result;
    }

    std::vector<const Order*> Get(OrderStatus status) override {
        std::vector<const Order*> result;
        const auto& index  = orders_.get<ByStatus>();
        auto [first, last] = index.equal_range(status);
        result.reserve(
            std::distance(first, last));  // ← резервируем нужное количество
        for (auto it = first; it != last; ++it) result.push_back(&*it);
        return result;
    }

    std::vector<const Order*> Get(Category category, OrderMode mode) override {
        std::vector<const Order*> result;
        const auto& index  = orders_.get<ByCategoryMode>();
        auto [first, last] = index.equal_range(std::make_tuple(category, mode));
        result.reserve(
            std::distance(first, last));  // ← резервируем нужное количество
        for (auto it = first; it != last; ++it) result.push_back(&*it);
        return result;
    }

    std::pair<bool, const Order*> Get(uint64_t order_id) override {
        const auto& index = orders_.get<ByOrderId>();
        auto it           = index.find(order_id);
        if (it != index.end()) return {true, &*it};
        return {false, nullptr};
    }

    void Add(Order&& order) override {
        orders_.insert(std::move(order));  // перемещение без копирования
    }

    template <typename... Args>
    void Emplace(Args&&... args) {
        orders_.emplace(std::forward<Args>(args)...);  // прямое размещение
    }

    void UpdateStatus(uint64_t order_id,
                      aoe::binance::OrderStatus new_order_status,
                      aoe::binance::PendingAction new_pending_action) override {
        auto& index = orders_.get<ByOrderId>();
        auto it     = index.find(order_id);
        if (it != index.end()) {
            index.modify(it, [&](Order& o) {
                o.order_status   = new_order_status;
                o.pending_action = new_pending_action;
            });
        }
    }
    void Remove(uint64_t order_id) override {
        auto& index = orders_.get<ByOrderId>();
        auto it     = index.find(order_id);
        if (it != index.end()) {
            index.erase(it);
        } else {
            // Можно добавить лог или обработку ошибки, если нужно
            // std::cerr << "Order with ID " << order_id << " not found.\n";
        }
    }
    void UpdateState(uint64_t order_id, double new_price,
                     double new_qty) override {
        auto& index = orders_.get<ByOrderId>();
        auto it     = index.find(order_id);
        if (it != index.end()) {
            index.modify(it, [&](Order& o) {
                o.price = new_price;
                o.qty   = new_qty;
            });
        }
    }

  private:
    OrderSet orders_;
};
};  // namespace impl
};  // namespace binance
};  // namespace aoe