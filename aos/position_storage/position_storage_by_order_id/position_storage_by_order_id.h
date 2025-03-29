#pragma once
#include <unordered_map>

namespace aos {
namespace impl {

template <typename Qty, template <typename> class MemoryPool, typename Uid>
class PositionStorageByOrderId
    : public IPositionStorageByOrderId<Qty, MemoryPool, Uid> {
    struct Key {
        common::ExchangeId exchange;
        common::TradingPair tradingPair;
        Uid uid;

        bool operator==(const Key& other) const {
            return exchange == other.exchange &&
                   tradingPair == other.tradingPair && uid == other.uid;
        }
    };

    struct KeyHasher {
        std::size_t operator()(const Key& k) const {
            std::size_t h1 = std::hash<common::ExchangeId>()(k.exchange);
            std::size_t h2 = std::hash<std::string>()(k.tradingPair);
            std::size_t h3 = std::hash<Uid>()(k.uid);

            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
    std::unordered_map<Key, Qty, KeyHasher> positions_;

  public:
    using Base = IPositionStorageByOrderId<Qty, MemoryPool, Uid>;

    PositionStorageByOrderId()           = default;
    ~PositionStorageByOrderId() override = default;

    std::pair<bool, Qty> GetPosition(common::ExchangeId exchange,
                                     common::TradingPair tradingPair,
                                     Uid uid) const override {
        auto it = positions_.find({exchange, tradingPair, uid});
        if (it == positions_.end()) {
            return {false, Qty{}};
        }
        return {true, it->second};
    }

    void AddPosition(common::ExchangeId exchange,
                     common::TradingPair& tradingPair, Qty qty,
                     Uid uid) override {
        positions_[{exchange, tradingPair, uid}] += qty;
    }

    bool RemovePosition(common::ExchangeId exchange,
                        common::TradingPair& tradingPair, Qty qty,
                        Uid uid) override {
        // auto key = std::make_tuple(exchange, tradingPair, uid);
        auto it = positions_.find({exchange, tradingPair, uid});
        if (it == positions_.end()) {
            return true;  //?
        }
        if (it->second - qty == 0) {
            positions_.erase(it);
            return true;
        }
        if (it->second - qty < 0) {
            positions_.erase(it);
            return true;  //?
        }
        it->second -= qty;
        return true;
    }
};
};  // namespace impl
};  // namespace aos
