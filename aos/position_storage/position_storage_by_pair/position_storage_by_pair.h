#pragma once

#include "aos/pnl/i_pnl.h"
#include "aos/position_storage/position_storage_by_pair/i_position_storage_by_pair.h"
#include "aos/trading_pair/trading_pair.h"
#include "boost/intrusive_ptr.hpp"
namespace aos {
namespace impl {

template <typename Price, typename Qty, typename PositionT>
class PositionStorageByPair
    : public PositionStorageByPairInterface<Price, Qty, PositionT> {
  private:
    using Key = std::pair<common::ExchangeId, aos::TradingPair>;
    struct KeyHash {
        std::size_t operator()(const Key& key) const {
            std::size_t hash_value = 0;

            std::size_t h1 =
                std::hash<int>{}(static_cast<int>(key.first));  // ExchangeId
            std::size_t h2 =
                std::hash<aos::TradingPair>{}(key.second);  // TradingPair

            boost::hash_combine(hash_value, h1);
            boost::hash_combine(hash_value, h2);

            return hash_value;
        }
    };
    std::unordered_map<Key, PositionT, KeyHash> storage_position_;
    std::function<PositionT()>
        position_factory_;  // 👈 теперь используем фабрику

  public:
    explicit PositionStorageByPair(std::function<PositionT()> position_factory)
        : position_factory_(std::move(position_factory)) {};
    ~PositionStorageByPair() override = default;
    std::optional<std::reference_wrapper<const PositionT>> GetPosition(
        common::ExchangeId exchange,
        aos::TradingPair trading_pair) const override {
        auto it = storage_position_.find({exchange, trading_pair});
        if (it == storage_position_.end()) {
            return std::nullopt;
        }
        return std::cref(it->second);  // const reference wrapper
    }

    void AddPosition(common::ExchangeId exchange, aos::TradingPair trading_pair,
                     Price price, Qty qty) override {
        Key key = {exchange, trading_pair};
        if (storage_position_.find(key) == storage_position_.end()) {
            storage_position_.emplace(key, position_factory_());
        }
        storage_position_.at(key).AddPosition(exchange, trading_pair, price,
                                              qty);
        logi("Adding position: {}, {}, Price={}, Qty={}", exchange,
             trading_pair, price, qty);
    }

    bool RemovePosition(common::ExchangeId exchange,
                        aos::TradingPair trading_pair, Price price,
                        Qty qty) override {
        Key key = {exchange, trading_pair};
        if (storage_position_.find(key) == storage_position_.end()) {
            // Do not create a position when trying to remove a non-existent
            // one. Returning false indicates that there was nothing to remove.
            return false;
        }
        storage_position_.at(key).RemovePosition(exchange, trading_pair, price,
                                                 qty);
        if (storage_position_.at(key).IsEmpty()) {
            storage_position_.erase(key);
        }
        logi("Removing position: {}, {}, Price={}, Qty={}", exchange,
             trading_pair, price, qty);
        return true;
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename PositionT, typename StrategyT>
class PositionStorageByPairDeprecated
    : public IPositionStorageByPair<Price, Qty, MemoryPool> {
  private:
    using Key         = std::pair<common::ExchangeId, aos::TradingPair>;
    using RealizedPnl = IRealizedPnlForTradingPair<Price, Qty>::RealizedPnl;
    struct KeyHash {
        std::size_t operator()(const Key& key) const {
            std::size_t hash_value = 0;

            std::size_t h1 =
                std::hash<int>{}(static_cast<int>(key.first));  // ExchangeId
            std::size_t h2 =
                std::hash<aos::TradingPair>{}(key.second);  // TradingPair

            boost::hash_combine(hash_value, h1);
            boost::hash_combine(hash_value, h2);

            return hash_value;
        }
    };
    std::unordered_map<Key, PositionT, KeyHash> storage_position_;
    boost::intrusive_ptr<StrategyT> strategy_;

  public:
    explicit PositionStorageByPairDeprecated(
        boost::intrusive_ptr<StrategyT> strategy)
        : strategy_(std::move(strategy)) {};
    std::pair<bool, Qty> GetPosition(
        common::ExchangeId exchange,
        aos::TradingPair tradingPair) const override {
        Key key = {exchange, tradingPair};
        auto it = storage_position_.find(key);
        if (it == storage_position_.end()) {
            return {false, Qty{}};
        }
        return {true, it->second.GetPosition()};
    }

    void AddPosition(common::ExchangeId exchange, aos::TradingPair trading_pair,
                     Price price, Qty qty) override {
        Key key = {exchange, trading_pair};
        if (storage_position_.find(key) == storage_position_.end()) {
            storage_position_.emplace(key, PositionT(strategy_));
        }
        storage_position_.at(key).AddPosition(exchange, trading_pair, price,
                                              qty);
        logi("Adding position: {}, {}, Price={}, Qty={}", exchange,
             trading_pair, price, qty);
    }

    bool RemovePosition(common::ExchangeId exchange,
                        aos::TradingPair trading_pair, Price price,
                        Qty qty) override {
        Key key = {exchange, trading_pair};
        if (storage_position_.find(key) == storage_position_.end()) {
            storage_position_.emplace(key, PositionT(strategy_));
        }
        storage_position_.at(key).RemovePosition(exchange, trading_pair, price,
                                                 qty);
        // storage_realized_pnl_.at(key) += realized_pnl;
        if (storage_position_.at(key).IsEmpty()) {
            storage_position_.erase(key);
        }
        logi("Removing position: {}, {}, Price={}, Qty={}", exchange,
             trading_pair, price, qty);
        return true;
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename PositionStorageByPairDeprecated, typename PositionT,
          typename StrategyT>
class PositionStorageBuilder {
    MemoryPool<PositionStorageByPairDeprecated>& pool_;
    boost::intrusive_ptr<StrategyT> strategy_;

  public:
    explicit PositionStorageBuilder(
        MemoryPool<PositionStorageByPairDeprecated>& pool)
        : pool_(pool) {}

    PositionStorageBuilder& SetPositionStrategy(
        boost::intrusive_ptr<StrategyT> strategy) {
        strategy_ = strategy;
        return *this;
    }

    auto Build() {
        if (!strategy_) {
            throw std::logic_error("Strategy not set in builder");
        }
        auto* obj = pool_.Allocate(strategy_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<
            IPositionStorageByPair<Price, Qty, MemoryPool>>(obj);
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename PositionStorageByPairT, typename PositionT,
          typename StrategyT>
class PositionStorageContainer {
    MemoryPool<PositionStorageByPairT> pool_;
    boost::intrusive_ptr<StrategyT> strategy_;

  public:
    explicit PositionStorageContainer(size_t size) : pool_(size) {}

    PositionStorageContainer& SetPositionStrategy(
        boost::intrusive_ptr<StrategyT> strategy) {
        strategy_ = strategy;
        return *this;
    }
    auto Build() {
        return PositionStorageBuilder<Price, Qty, MemoryPool,
                                      PositionStorageByPairT, PositionT,
                                      StrategyT>(pool_)
            .SetPositionStrategy(strategy_)
            .Build();
    }
};

};  // namespace impl

};  // namespace aos