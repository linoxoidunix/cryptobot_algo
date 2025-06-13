#pragma once
#include <cstdint>


namespace aos {
namespace strategies {
namespace deviation_and_mutual_information {
template <typename HashT>
struct Config{
    const uint32_t window_size;
    const uint32_t number_bins;
    const HashT allowed_asset_hash;
    // constexpr-конструктор
    constexpr Config(uint32_t new_window_size, uint32_t new_number_bins, HashT new_allowed_asset_hash)
        : window_size(new_window_size), number_bins(new_number_bins), allowed_asset_hash(new_allowed_asset_hash) {}
 
};
};
};
};
