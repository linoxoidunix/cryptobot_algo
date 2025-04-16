#pragma once
namespace common {
// template <template <typename> typename Pool, typename... Ts>
// class MultiPoolHolder {
//   public:
//     explicit MultiPoolHolder(std::size_t pool_size)
//         : pools_(Pool<Ts>(pool_size)...) {}

//     template <typename T>
//     Pool<T>& Get() {
//         return std::get<Pool<T>>(pools_);
//     }

//   private:
//     std::tuple<Pool<Ts>...> pools_;
// };
// template <template <typename> class Pool, typename... Ts>
// struct MultiPoolHolder {
//     std::tuple<Pool<Ts>...> pools_;

//     explicit MultiPoolHolder(std::size_t pool_size)
//         : pools_(make_pools(pool_size, std::index_sequence_for<Ts...>{})) {}
//     template <typename T>
//     Pool<T>& Get() {
//         return std::get<Pool<T>>(pools_);
//     }

//   private:
//     template <std::size_t... Is>
//     static std::tuple<Pool<Ts>...> make_pools(std::size_t size,
//                                               std::index_sequence<Is...>) {
//         return std::make_tuple(Pool<Ts>(size)...);
//     }
// };

};  // namespace common
