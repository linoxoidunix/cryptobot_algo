#pragma once
#include "aos/pnl/realized_storage/i_pnl_realized_storage.h"
namespace aos {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class RealizedPnlStorage : public IPnlRealizedStorage<Price, Qty, MemoryPool> {
  private:
    using RealizedPnl =
        IPnlRealizedStorage<Price, Qty, MemoryPool>::RealizedPnl;

    using Key = std::pair<common::ExchangeId, common::TradingPair>;
    struct KeyHash {
        std::size_t operator()(const Key& key) const {
            std::size_t hash_value = 0;

            std::size_t h1         = std::hash<common::ExchangeId>{}(key.first);
            std::size_t h2         = common::TradingPairHash{}(key.second);

            boost::hash_combine(hash_value, h1);
            boost::hash_combine(hash_value, h2);

            return hash_value;
        }
    };
    std::unordered_map<Key, Price, KeyHash> realized_pnl_;

  public:
    void Add(common::ExchangeId exchange, common::TradingPair tradingPair,
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
        common::TradingPair tradingPair) const override {
        Key key = {exchange, tradingPair};
        auto it = realized_pnl_.find(key);
        if (it == realized_pnl_.end()) {
            return {false, Price{}};
        }
        return {true, it->second};
    }
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