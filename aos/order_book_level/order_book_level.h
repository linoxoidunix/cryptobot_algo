#pragma once
#include "boost/intrusive/avltree.hpp"

namespace aos {
template <typename Price, typename Qty>
struct OrderBookLevel {
    Price price;
    Qty qty;
    boost::intrusive::avl_set_member_hook<> member_hook;
    friend bool operator<(const OrderBookLevel &a, const OrderBookLevel &b) {
        return a.price < b.price;
    }
    friend bool operator>(const OrderBookLevel &a, const OrderBookLevel &b) {
        return a.price > b.price;
    }
    friend bool operator==(const OrderBookLevel &a, const OrderBookLevel &b) {
        return a.price == b.price;
    }
    OrderBookLevel() = default;
    OrderBookLevel(Price new_price, Qty new_qty)
        : price(new_price), qty(new_qty) {}
};
};  // namespace aos