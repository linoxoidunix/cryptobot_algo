#pragma once
#include "aos/logger/mylog.h"

namespace aos {
enum class Ticker {  // NOLINT
    kBTC,
    kETH,
    kUSDT,
    kBNB,
    kSOL,
    kXRP,
    kADA,
    kCount  // для удобства
};
};

#include "magic_enum/magic_enum.hpp"
template <>
class fmt::formatter<aos::Ticker> {
  public:
    static constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }
    template <typename Context>
    constexpr auto format(const aos::Ticker& foo, Context& ctx) const {
        return fmt::format_to(ctx.out(), "Ticker:{}",
                              magic_enum::enum_name(foo));
    }
};