#pragma once
#include <cstdint>

namespace aos {
enum class MarketType : uint8_t {
    Spot,
    Margin,  // Cross
    IsolatedMargin
};
};

#include "magic_enum/magic_enum.hpp"
template <>
class fmt::formatter<aos::MarketType> {
  public:
    static constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }
    template <typename Context>
    constexpr auto format(const aos::MarketType& foo, Context& ctx) const {
        return fmt::format_to(ctx.out(), "MaketType:{}",
                              magic_enum::enum_name(foo));
    }
};