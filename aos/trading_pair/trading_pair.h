#pragma once

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