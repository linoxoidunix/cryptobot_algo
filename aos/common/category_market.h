#pragma once
#include <cstdint>

namespace aos {
/**
 * @brief Market category type identifier (raw underlying type).
 * 
 * Represents the type of market such as:
 * - Spot trading
 * - Futures/Perpetual contracts
 * - Options contracts
 * - Other derivative products
 * 
 * The actual values should correspond to enums defined in the bybit enums.
 */
using CategoryRaw = uint8_t;

}; // namespace aos