#pragma once

namespace aos {

/**
 * @brief Flags to enable or disable various notifiers.
 *
 * Each boolean flag controls whether a specific notifier is active.
 * Setting a flag to true enables the corresponding notifier,
 * while setting it to false disables it.
 */
struct NotifierFlags {
    /// Enables or disables the Best Bid notifier. Set to true to enable.
    bool best_bid_enabled;

    /// Enables or disables the Best Ask notifier. Set to true to enable.
    bool best_ask_enabled;

    /// Enables or disables the Best Bid Price notifier. Set to true to enable.
    bool best_bid_price_enabled;

    /// Enables or disables the Best Ask Price notifier. Set to true to enable.
    bool best_ask_price_enabled;

    constexpr bool operator==(const NotifierFlags&) const = default;
};

}  // namespace aos
