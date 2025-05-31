#pragma once
namespace aoe {
namespace bybit {
namespace spot {
template <typename Price, typename Qty>
struct OrderFilterInterface {
    using Amount = decltype(std::declval<Price>() * std::declval<Qty>());
    Qty min_order_qty{};
    Qty max_order_qty{};
    Amount min_order_amt{};
    Amount max_order_amt{};
    Qty base_precission{};
    Qty quote_precission{};
    Price tick_size{};
    virtual ~OrderFilterInterface() = default;
};

}  // namespace spot

namespace linear {
template <typename Price, typename Qty>
struct OrderFilterInterface {
    using Amount = decltype(std::declval<Price>() * std::declval<Qty>());
    Qty min_order_qty{};
    Qty max_order_qty{};
    Amount min_order_amt{};
    Amount max_order_amt{};
    Qty base_precission{};
    Qty quote_precission{};
    Price tick_size{};
    virtual ~OrderFilterInterface() = default;
};

}  // namespace linear
};  // namespace bybit
};  // namespace aoe