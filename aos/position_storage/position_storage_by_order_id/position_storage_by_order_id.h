#pragma once
#include <unordered_map>

namespace aos {
namespace impl {

class PositionStorageByOrderIdDefault
    : public PositionStorageByOrderIdInterface {
    struct Key {
        common::ExchangeId exchange;
        common::TradingPair tradingPair;
        size_t uid;

        bool operator==(const Key& other) const {
            return exchange == other.exchange &&
                   tradingPair == other.tradingPair && uid == other.uid;
        }
    };

    struct KeyHasher {
        std::size_t operator()(const Key& k) const {
            std::size_t hash_value = 0;

            std::size_t h1 = std::hash<common::ExchangeId>{}(k.exchange);
            std::size_t h2 = common::TradingPairHash{}(k.tradingPair);
            std::size_t h3 = std::hash<std::size_t>{}(k.uid);

            boost::hash_combine(hash_value, h1);
            boost::hash_combine(hash_value, h2);
            boost::hash_combine(hash_value, h3);

            return hash_value;
        }
    };
    std::unordered_map<Key, double, KeyHasher> positions_;

  public:
    PositionStorageByOrderIdDefault()           = default;
    ~PositionStorageByOrderIdDefault() override = default;

    std::pair<bool, double> GetPosition(common::ExchangeId exchange,
                                        common::TradingPair tradingPair,
                                        std::size_t uid) const override {
        auto it = positions_.find({exchange, tradingPair, uid});
        if (it == positions_.end()) {
            return {false, double{}};
        }
        return {true, it->second};
    }

    void AddPosition(common::ExchangeId exchange,
                     common::TradingPair& tradingPair, double qty,
                     std::size_t uid) override {
        positions_[{exchange, tradingPair, uid}] += qty;
    }

    bool RemovePosition(common::ExchangeId exchange,
                        common::TradingPair& tradingPair, double qty,
                        std::size_t uid) override {
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
            std::size_t hash_value = 0;

            std::size_t h1 = std::hash<common::ExchangeId>{}(k.exchange);
            std::size_t h2 = common::TradingPairHash{}(k.tradingPair);
            std::size_t h3 = std::hash<Uid>{}(k.uid);

            boost::hash_combine(hash_value, h1);
            boost::hash_combine(hash_value, h2);
            boost::hash_combine(hash_value, h3);

            return hash_value;
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
