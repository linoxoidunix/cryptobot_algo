#pragma once
#include <cstdint>

namespace aos {
namespace strategies {
namespace deviation_and_mutual_information {
template <typename HashT>
struct Config {
    const uint32_t window_size;
    const uint32_t number_bins;
    const HashT allowed_asset_hash;
    const double deviation_ratio_threshold;
    const double mi_threshold;
    // constexpr-конструктор
    constexpr Config(uint32_t new_window_size, uint32_t new_number_bins,
                     HashT new_allowed_asset_hash, double new_deviation_ratio_threshold,
                    double new_mi_threshold)
        : window_size(new_window_size),
          number_bins(new_number_bins),
          allowed_asset_hash(new_allowed_asset_hash),
          deviation_ratio_threshold(new_deviation_ratio_threshold),
          mi_threshold(new_mi_threshold) {}
};
};  // namespace deviation_and_mutual_information
};  // namespace strategies
};  // namespace aos
