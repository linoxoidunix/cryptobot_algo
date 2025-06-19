#pragma once
#include "aos/pnl/realized_calculator/i_pnl_realized_calculator.h"
#include "aos/pnl/realized_storage/i_pnl_realized_storage.h"
#include "boost/functional/hash.hpp"
namespace aos {
namespace impl {

template <typename Price, typename Qty>
class RealizedPnlStorageDefault
    : public PnlRealizedStorageInterface<Price, Qty> {
  private:
    using RealizedPnl = PnlRealizedStorageInterface<Price, Qty>::RealizedPnl;

    using Key         = std::pair<common::ExchangeId, aos::TradingPair>;
    struct KeyHash {
        std::size_t operator()(const Key& key) const {
            std::size_t hash_value = 0;

            std::size_t h1         = std::hash<common::ExchangeId>{}(key.first);
            std::size_t h2         = std::hash<aos::TradingPair>{}(key.second);

            boost::hash_combine(hash_value, h1);
            boost::hash_combine(hash_value, h2);

            return hash_value;
        }
    };
    RealizedPnlCalculatorInterface<Price, Qty>& realized_pnl_calculator_;
    std::unordered_map<Key, Price, KeyHash> realized_pnl_;

  public:
    explicit RealizedPnlStorageDefault(
        RealizedPnlCalculatorInterface<Price, Qty>& realized_pnl_calculator)
        : realized_pnl_calculator_(realized_pnl_calculator) {};
    RealizedPnl FixProfit(common::ExchangeId exchange,
                          aos::TradingPair tradingPair, Price avg_price,
                          Qty net_qty, Price price, Qty qty) override {
        auto pnl =
            realized_pnl_calculator_.Calculate(avg_price, net_qty, price, qty);
        Key key = {exchange, tradingPair};
        auto old_pnl =
            (realized_pnl_.contains(key)) ? realized_pnl_.at(key) : 0;
        realized_pnl_[key] += pnl;
        logi("Updated RealizedPnl for {}, {}. New pnl: {} Old pnl:{}", exchange,
             tradingPair, realized_pnl_.at(key), old_pnl);
        return pnl;
    }

    std::pair<bool, RealizedPnl> GetRealizedPnl(
        common::ExchangeId exchange,
        aos::TradingPair tradingPair) const override {
        Key key = {exchange, tradingPair};
        auto it = realized_pnl_.find(key);
        if (it == realized_pnl_.end()) {
            return {false, Price{}};
        }
        return {true, it->second};
    }
    ~RealizedPnlStorageDefault() override = default;
};
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class RealizedPnlStorage : public IPnlRealizedStorage<Price, Qty, MemoryPool> {
  private:
    using RealizedPnl =
        IPnlRealizedStorage<Price, Qty, MemoryPool>::RealizedPnl;

    using Key = std::pair<common::ExchangeId, aos::TradingPair>;
    struct KeyHash {
        std::size_t operator()(const Key& key) const {
            std::size_t hash_value = 0;

            std::size_t h1         = std::hash<common::ExchangeId>{}(key.first);
            std::size_t h2         = std::hash<aos::TradingPair>{}(key.second);

            boost::hash_combine(hash_value, h1);
            boost::hash_combine(hash_value, h2);

            return hash_value;
        }
    };
    std::unordered_map<Key, Price, KeyHash> realized_pnl_;

  public:
    void Add(common::ExchangeId exchange, aos::TradingPair tradingPair,
             RealizedPnl pnl) override {
        Key key = {exchange, tradingPair};
        auto old_pnl =
            (realized_pnl_.contains(key)) ? realized_pnl_.at(key) : 0;
        realized_pnl_[key] += pnl;
        logi("Updated RealizedPnl for {}, {}. New pnl: {} Old pnl:{}", exchange,
             tradingPair, realized_pnl_.at(key), old_pnl);
    }

    std::pair<bool, RealizedPnl> GetRealizedPnl(
        common::ExchangeId exchange,
        aos::TradingPair tradingPair) const override {
        Key key = {exchange, tradingPair};
        auto it = realized_pnl_.find(key);
        if (it == realized_pnl_.end()) {
            return {false, Price{}};
        }
        return {true, it->second};
    }
    ~RealizedPnlStorage() override = default;
};

// === Билдер для RealizedPnlStorage ===
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename RealizedPnlStorageT>
class RealizedPnlStorageBuilder {
    MemoryPool<RealizedPnlStorageT>& pool_;

  public:
    using StorageType = IPnlRealizedStorage<Price, Qty, MemoryPool>;

    explicit RealizedPnlStorageBuilder(MemoryPool<RealizedPnlStorageT>& pool)
        : pool_(pool) {}

    auto Build() {
        auto* obj = pool_.Allocate();
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<StorageType>(obj);
    }
};

// === Контейнер для RealizedPnlStorage ===
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename RealizedPnlStorageT>
class RealizedPnlStorageContainer {
    MemoryPool<RealizedPnlStorageT> pool_;
    using Builder =
        RealizedPnlStorageBuilder<Price, Qty, MemoryPool, RealizedPnlStorageT>;

  public:
    explicit RealizedPnlStorageContainer(size_t size) : pool_(size) {}

    auto Build() {
        Builder builder(pool_);
        return builder.Build();
    }
};

}  // namespace impl
}  // namespace aos