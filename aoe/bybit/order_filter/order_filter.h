// #pragma once
// namespace aoe {
// namespace bybit {
// template <typename Price, typename Qty>
// struct OrderFilter {
//     using Amount = decltype(std::declval<Price>() * std::declval<Qty>());

//     Qty min_order_qty{};
//     Qty max_order_qty{};
//     Amount min_order_amt{};
//     Amount max_order_amt{};
//     Qty base_precission{};
//     Qty quote_precission{};
//     Price tick_size{};
// };
// };  // namespace bybit
// };  // namespace aoe