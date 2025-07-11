#pragma once
#include <cstdint>

#include "aos/logger/mylog.h"

namespace aos {
enum class TradingPair : uint32_t {  // NOLINT
    kBTCUSDT,
    kETHUSDT,
    kETHBTC,
    kBTCETH,
    kBNBUSDT,
    kBNBETH,
    kETHBNB,
    kSOLUSDT,
    kSOLBNB,
    kXRPUSDT,
    kADAUSDT,
    kADAXRP,
    kCount  // для удобства
};
};

namespace aos {
using TradingPairRaw = std::underlying_type_t<TradingPair>;
}

namespace std {
template <>
struct hash<aos::TradingPair> {
    std::underlying_type_t<aos::TradingPair> operator()(
        aos::TradingPair pair) const {
        return static_cast<std::underlying_type_t<aos::TradingPair>>(pair);
    }
};
};  // namespace std

#include "magic_enum/magic_enum.hpp"
template <>
class fmt::formatter<aos::TradingPair> {
  public:
    static constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }
    template <typename Context>
    constexpr auto format(const aos::TradingPair& foo, Context& ctx) const {
        return fmt::format_to(ctx.out(), "TradingPair:{}",
                              magic_enum::enum_name(foo));
    }
};