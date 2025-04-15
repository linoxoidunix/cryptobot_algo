#pragma once
namespace common {
template <template <typename> typename Pool, typename... Ts>
class MultiPoolHolder {
  public:
    explicit MultiPoolHolder(std::size_t pool_size)
        : pools_(Pool<Ts>(pool_size)...) {}

    template <typename T>
    Pool<T>& Get() {
        return std::get<Pool<T>>(pools_);
    }

  private:
    std::tuple<Pool<Ts>...> pools_;
};
};  // namespace common
