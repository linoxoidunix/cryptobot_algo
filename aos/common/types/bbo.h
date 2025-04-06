#pragma once
namespace aos {
template <typename Price>
struct BBO {
    Price best_bid;
    Price best_ask;
    bool operator==(const BBO& other) const {
        return best_bid == other.best_bid && best_ask == other.best_ask;
    }
};
};  // namespace aos