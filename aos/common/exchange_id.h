#pragma once
#include <cstdint>
#include <functional>
#include "fmt/core.h"
#include "fmt/format.h"         // важно
#include "magic_enum/magic_enum.hpp"       // нужно для magic_enum::enum_name

namespace common{
enum class ExchangeId : uint16_t{ kBinance, kBybit, kMexc, kInvalid };
}

namespace common{
    using ExchangeIdRaw      = std::underlying_type_t<ExchangeId>;
}

template <>
class fmt::formatter<common::ExchangeId> {
  public:
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template <typename Context>
    constexpr auto format(const common::ExchangeId& foo, Context& ctx) const {
        return fmt::format_to(ctx.out(), "Exchange:{}",
                              magic_enum::enum_name(foo));
    }
};

namespace std {
template <>
struct hash<common::ExchangeId> {
    std::underlying_type_t<common::ExchangeId> operator()(const common::ExchangeId& id) const noexcept {
        return std::hash<std::underlying_type_t<common::ExchangeId>>{}(
            static_cast<std::underlying_type_t<common::ExchangeId>>(id));
    }
};
};
