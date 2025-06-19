#pragma once

#include <unordered_map>

#include "aos/logger/mylog.h"
#include "aos/market_triplet_manager/i_market_triplet_manager.h"

namespace aos {
namespace impl {
// 📌 Реализация подписок (Single Responsibility - S)
template <typename HashT>
class MarketTripletManagerDefault
    : public MarketTripletManagerInterface<HashT> {
  public:
    void Connect(const HashT& asset_x, const HashT& asset_y) override {
        pairs_[asset_x].insert(asset_y);
        pairs_[asset_y].insert(asset_x);
    }

    const std::unordered_set<HashT>& GetPairs(
        const HashT& asset) const override {
        static const std::unordered_set<HashT> kEmptySet;
        return pairs_.contains(asset) ? pairs_.at(asset) : kEmptySet;
    }

    bool HasPair(const HashT& asset) const override {
        return pairs_.contains(asset);
    }
    ~MarketTripletManagerDefault() override {
        logi("{}", "MarketTripletManagerDefault dtor");
    }

  private:
    std::unordered_map<HashT, std::unordered_set<HashT>> pairs_;
};

template <class T, template <typename> typename MemoryPoolNotThreadSafety>
class MarketTripletManager
    : public aos::IMarketTripletManager<T, MemoryPoolNotThreadSafety> {
  public:
    void Connect(const T& asset_x, const T& asset_y) override {
        pairs_[asset_x].insert(asset_y);
        pairs_[asset_y].insert(asset_x);
    }

    const std::unordered_set<T>& GetPairs(const T& asset) const override {
        static const std::unordered_set<T> kEmptySet;
        return pairs_.contains(asset) ? pairs_.at(asset) : kEmptySet;
    }

    bool HasPair(const T& asset) const override {
        return pairs_.contains(asset);
    }
    ~MarketTripletManager() override {
        logi("{}", "MarketTripletManager dtor");
    }
    static boost::intrusive_ptr<
        MarketTripletManager<T, MemoryPoolNotThreadSafety>>
    Create(MemoryPoolNotThreadSafety<
           MarketTripletManager<T, MemoryPoolNotThreadSafety>>& pool) {
        auto* obj = pool.Allocate();
        obj->SetMemoryPool(&pool);
        return boost::intrusive_ptr<
            MarketTripletManager<T, MemoryPoolNotThreadSafety>>(obj);
    }

  private:
    std::unordered_map<T, std::unordered_set<T>> pairs_;
};
};  // namespace impl
};  // namespace aos