#pragma once
#include "fmtlog.h"

namespace aos {
enum class TradingPair {
    kBTCUSDT,
    kETHUSDT,
    kSOLUSDT,
    kCount  // для удобства
};
};

namespace std {
template <>
struct hash<aos::TradingPair> {
    std::size_t operator()(aos::TradingPair pair) const {
        return static_cast<std::size_t>(pair);
    }
};
};  // namespace std

#include "magic_enum/magic_enum.hpp"
template <>
class fmt::formatter<aos::TradingPair> {
  public:
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template <typename Context>
    constexpr auto format(const aos::TradingPair& foo, Context& ctx) const {
        return fmt::format_to(ctx.out(), "TradingPair[{}]",
                              magic_enum::enum_name(foo));
    }
};