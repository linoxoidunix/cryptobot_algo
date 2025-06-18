#pragma once
#include <cstdint>

#include "fmtlog.h"                   // NOLINT
#include "magic_enum/magic_enum.hpp"  // нужно для magic_enum::enum_name

namespace aos {
enum class NetworkEnvironment : uint8_t { kMainNet, kTestNet };
};

namespace aos {
using NetworkEnvironmentRaw = std::underlying_type_t<NetworkEnvironment>;
}

namespace std {
template <>
struct hash<aos::NetworkEnvironment> {
    std::underlying_type_t<aos::NetworkEnvironment> operator()(
        aos::NetworkEnvironment pair) const {
        return static_cast<std::underlying_type_t<aos::NetworkEnvironment>>(
            pair);
    }
};
};  // namespace std

template <>
class fmt::formatter<aos::NetworkEnvironment> {
  public:
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template <typename Context>
    constexpr auto format(const aos::NetworkEnvironment& foo,
                          Context& ctx) const {
        return fmt::format_to(ctx.out(), "NetworkEnv:{}",
                              magic_enum::enum_name(foo));
    }
};
