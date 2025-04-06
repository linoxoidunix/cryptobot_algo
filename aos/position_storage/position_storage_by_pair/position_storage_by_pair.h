#pragma once

#include "aos/pnl/i_pnl.h"
#include "aos/position_storage/position_storage_by_pair/i_position_storage_by_pair.h"
#include "boost/intrusive_ptr.hpp"
namespace aos {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename PositionT, typename StrategyT>
class PositionStorageByPair
    : public IPositionStorageByPair<Price, Qty, MemoryPool> {
  private:
    using Key         = std::pair<common::ExchangeId, common::TradingPair>;
    using RealizedPnl = IRealizedPnlForTradingPair<Price, Qty>::RealizedPnl;
    struct KeyHash {
        std::size_t operator()(const Key& key) const {
            std::size_t hash_value = 0;

            std::size_t h1 =
                std::hash<int>{}(static_cast<int>(key.first));  // ExchangeId
            std::size_t h2 =
                common::TradingPairHash{}(key.second);  // TradingPair

            boost::hash_combine(hash_value, h1);
            boost::hash_combine(hash_value, h2);

            return hash_value;
        }
    };
    std::unordered_map<Key, PositionT, KeyHash> storage_position_;
    boost::intrusive_ptr<StrategyT> strategy_;

  public:
    PositionStorageByPair(boost::intrusive_ptr<StrategyT> strategy)
        : strategy_(strategy) {};
    std::pair<bool, Qty> GetPosition(
        common::ExchangeId exchange,
        common::TradingPair tradingPair) const override {
        Key key = {exchange, tradingPair};
        auto it = storage_position_.find(key);
        if (it == storage_position_.end()) {
            return {false, Qty{}};
        }
        return {true, it->second.GetPosition()};
    }

    void AddPosition(common::ExchangeId exchange,
                     common::TradingPair trading_pair, Price price,
                     Qty qty) override {
        Key key = {exchange, trading_pair};
        if (storage_position_.find(key) == storage_position_.end()) {
            storage_position_.emplace(
                key,
                PositionT(strategy_));  // TODO: Передавать стратегию
        }
        storage_position_.at(key).AddPosition(exchange, trading_pair, price,
                                              qty);
        logi("Adding position: {}, {}, Price={}, Qty={}", exchange,
             trading_pair, price, qty);
    }

    bool RemovePosition(common::ExchangeId exchange,
                        common::TradingPair trading_pair, Price price,
                        Qty qty) override {
        Key key = {exchange, trading_pair};
        if (storage_position_.find(key) == storage_position_.end()) {
            storage_position_.emplace(
                key,
                PositionT(strategy_));  // TODO: Передавать стратегию
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
          typename PositionT, typename StrategyT>
class PositionStorageBuilder {
    MemoryPool<PositionT>& pool_;
    boost::intrusive_ptr<StrategyT> strategy_;

  public:
    explicit PositionStorageBuilder(MemoryPool<PositionT>& pool,
                                    boost::intrusive_ptr<StrategyT> strategy)
        : pool_(pool), strategy_(strategy) {}

    auto Build() {
        auto* obj = pool_.Allocate(strategy_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<PositionT>(obj);
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename PositionT, typename StrategyT>
class PositionStorageContainer {
    MemoryPool<PositionT> pool_;
    boost::intrusive_ptr<StrategyT> strategy_;

  public:
    explicit PositionStorageContainer(size_t size,
                                      boost::intrusive_ptr<StrategyT> strategy)
        : pool_(size), strategy_(strategy) {}

    auto Build() {
        PositionStorageBuilder<Price, Qty, MemoryPool, PositionT, StrategyT>
            builder(pool_, strategy_);
        return builder.Build();
    }
};

};  // namespace impl

};  // namespace aos