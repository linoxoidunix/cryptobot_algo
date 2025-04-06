#pragma once
#include <boost/intrusive_ptr.hpp>
#include <cassert>
#include <unordered_map>

#include "aos/common/types.h"
#include "aos/common/types/position_info.h"
#include "aos/pnl/unrealized_calculator/i_pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/i_pnl_unrealized_storage.h"
#include "aot/common/types.h"

namespace aos {
namespace impl {
// просто заменить значение
struct NetModePolicy {
    template <typename Map, typename Key, typename Value>
    static void Update(Map& m, const Key& key, const Value& value) {
        m[key] = value;
    }
};

// прибавить к текущему значению
struct HedgeModePolicy {
    template <typename Map, typename Key, typename Value>
    static void Update(Map& m, const Key& key, const Value& value) {
        m[key] += value;
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename UpdatePolicy = NetModePolicy>
class NetUnRealizedPnlStorage
    : public IPnlUnRealizedStorage<Price, Qty, MemoryPool> {
    using UnRealizedPnl =
        IPnlUnRealizedStorage<Price, Qty, MemoryPool>::UnRealizedPnl;

    using Key = std::pair<common::ExchangeId, common::TradingPair>;
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
    std::unordered_map<Key, PositionInfo<Price, Qty>, KeyHash> position_info_;
    std::unordered_map<Key, BBO<Price>, KeyHash> bbo_;
    std::unordered_map<Key, UnRealizedPnl, KeyHash> unrealized_pnl_;
    boost::intrusive_ptr<IUnRealizedPnlCalculator<Price, Qty, MemoryPool>>
        pnl_unrealized_calculator_;

  public:
    NetUnRealizedPnlStorage(
        boost::intrusive_ptr<IUnRealizedPnlCalculator<Price, Qty, MemoryPool>>
            pnl_unrealized_calculator)
        : pnl_unrealized_calculator_(pnl_unrealized_calculator) {}
    ~NetUnRealizedPnlStorage() = default;
    void UpdatePosition(common::ExchangeId exchange,
                        common::TradingPair tradingPair, Price avg_price,
                        Qty net_qty) override {
        Key key                       = {exchange, tradingPair};
        position_info_[key].avg_price = avg_price;
        position_info_[key].net_qty   = net_qty;
        auto bbo_it                   = bbo_.find(key);
        (bbo_it != bbo_.end())
                              ? UpdatePolicy::Update(
                  unrealized_pnl_, key,
                  pnl_unrealized_calculator_->Calculate(
                      avg_price, net_qty, bbo_it->second.best_bid,
                      bbo_it->second.best_ask))
                              : void();
    }

    void UpdateBBO(common::ExchangeId exchange, common::TradingPair tradingPair,
                   Price price_bid, Price price_ask) {
        Key key = {exchange, tradingPair};
        bbo_.try_emplace(key, price_bid, price_ask);
        auto pos_it = position_info_.find(key);
        (pos_it != position_info_.end())
            ? UpdatePolicy::Update(
                  unrealized_pnl_, key,
                  pnl_unrealized_calculator_->Calculate(
                      pos_it->second.avg_price, pos_it->second.net_qty,
                      price_bid, price_ask))
            : void();
    }

    std::pair<bool, UnRealizedPnl> GetUnRealizedPnl(
        common::ExchangeId exchange,
        common::TradingPair tradingPair) const override {
        Key key = {exchange, tradingPair};
        auto it = unrealized_pnl_.find(key);
        if (it == unrealized_pnl_.end()) {
            return {false, Price{}};
        }
        return {true, it->second};
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename UpdatePolicy = HedgeModePolicy>
class HedgedUnRealizedPnlStorage
    : public IPnlUnRealizedStorage<Price, Qty, MemoryPool> {
    using UnRealizedPnl =
        IPnlUnRealizedStorage<Price, Qty, MemoryPool>::UnRealizedPnl;

    using Key = std::pair<common::ExchangeId, common::TradingPair>;
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
    std::unordered_map<Key, PositionInfo<Price, Qty>, KeyHash>
        long_position_info_;
    std::unordered_map<Key, PositionInfo<Price, Qty>, KeyHash>
        short_position_info_;
    std::unordered_map<Key, BBO<Price>, KeyHash> bbo_;
    std::unordered_map<Key, UnRealizedPnl, KeyHash> unrealized_pnl_;
    boost::intrusive_ptr<IUnRealizedPnlCalculator<Price, Qty, MemoryPool>>
        pnl_unrealized_calculator_;

  public:
    HedgedUnRealizedPnlStorage(
        boost::intrusive_ptr<IUnRealizedPnlCalculator<Price, Qty, MemoryPool>>
            pnl_unrealized_calculator)
        : pnl_unrealized_calculator_(pnl_unrealized_calculator) {}
    ~HedgedUnRealizedPnlStorage() = default;
    void UpdatePosition(common::ExchangeId exchange,
                        common::TradingPair tradingPair, Price avg_price,
                        Qty net_qty) override {
        Key key = {exchange, tradingPair};
        if (net_qty >= 0) {
            long_position_info_[key].avg_price = avg_price;
            long_position_info_[key].net_qty   = net_qty;
        } else {
            short_position_info_[key].avg_price = avg_price;
            short_position_info_[key].net_qty   = net_qty;
        }
        // Проверяем наличие BBO
        auto bbo_it = bbo_.find(key);
        if (bbo_it == bbo_.end()) return;

        // Обновляем PnL для длинной позиции
        auto long_pos_it  = long_position_info_.find(key);
        auto short_pos_it = short_position_info_.find(key);
        if (long_pos_it != long_position_info_.end() ||
            short_pos_it != short_position_info_.end()) {
            unrealized_pnl_[key] = 0;
        } else {
            return;
        }
        if (long_pos_it != long_position_info_.end()) {
            UpdatePolicy::Update(
                unrealized_pnl_, key,
                pnl_unrealized_calculator_->Calculate(
                    long_pos_it->second.avg_price, long_pos_it->second.net_qty,
                    bbo_it->second.best_bid, bbo_it->second.best_ask));
        }
        // Обновляем PnL для короткой позиции
        if (short_pos_it != short_position_info_.end()) {
            UpdatePolicy::Update(
                unrealized_pnl_, key,
                pnl_unrealized_calculator_->Calculate(
                    short_pos_it->second.avg_price,
                    short_pos_it->second.net_qty, bbo_it->second.best_bid,
                    bbo_it->second.best_ask));
        }
    }

    void UpdateBBO(common::ExchangeId exchange, common::TradingPair tradingPair,
                   Price price_bid, Price price_ask) {
        Key key = {exchange, tradingPair};
        bbo_.try_emplace(key, price_bid, price_ask);
        // Обновляем PnL для длинной позиции
        auto long_pos_it  = long_position_info_.find(key);
        auto short_pos_it = short_position_info_.find(key);
        if (long_pos_it != long_position_info_.end() ||
            short_pos_it != short_position_info_.end()) {
            unrealized_pnl_[key] = 0;
        } else {
            return;
        }
        // calculate pnl for long
        if (long_pos_it != long_position_info_.end()) {
            UpdatePolicy::Update(
                unrealized_pnl_, key,
                pnl_unrealized_calculator_->Calculate(
                    long_pos_it->second.avg_price, long_pos_it->second.net_qty,
                    price_bid, price_ask));
        }
        // calculate pnl for short and sum it

        if (short_pos_it != short_position_info_.end()) {
            UpdatePolicy::Update(
                unrealized_pnl_, key,
                pnl_unrealized_calculator_->Calculate(
                    short_pos_it->second.avg_price,
                    short_pos_it->second.net_qty, price_bid, price_ask));
        }
    }

    std::pair<bool, UnRealizedPnl> GetUnRealizedPnl(
        common::ExchangeId exchange,
        common::TradingPair tradingPair) const override {
        Key key = {exchange, tradingPair};
        auto it = unrealized_pnl_.find(key);
        if (it == unrealized_pnl_.end()) {
            return {false, Price{}};
        }
        return {true, it->second};
    }
};

// === Билдер для UnRealizedPnlStorage ===
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename UnRealizedPnlStorageT>
class UnRealizedPnlStorageBuilder {
    MemoryPool<UnRealizedPnlStorageT>& pool_;
    boost::intrusive_ptr<IUnRealizedPnlCalculator<Price, Qty, MemoryPool>>
        pnl_unrealized_calculator_;

  public:
    explicit UnRealizedPnlStorageBuilder(
        MemoryPool<UnRealizedPnlStorageT>& pool)
        : pool_(pool) {}

    UnRealizedPnlStorageBuilder& SetPnlUnrealizedCalculator(
        boost::intrusive_ptr<IUnRealizedPnlCalculator<Price, Qty, MemoryPool>>
            pnl_unrealized_calculator) {
        pnl_unrealized_calculator_ = pnl_unrealized_calculator;
        return *this;
    }

    auto Build() {
        assert(pnl_unrealized_calculator_ &&
               "UnRealizedPnlCalculator must be set before Build()");
        if (!pnl_unrealized_calculator_) {
            throw std::logic_error(
                "UnRealizedPnlCalculator not set in builder");
        }
        auto* obj = pool_.Allocate(pnl_unrealized_calculator_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<
            IPnlUnRealizedStorage<Price, Qty, MemoryPool>>(obj);
    }
};

// === Контейнер для UnRealizedPnlStorage ===
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename UnrealizedPnlCalculatorContainerT,
          typename UnRealizedPnlStorageT>
class UnRealizedPnlStorageContainer {
    MemoryPool<UnRealizedPnlStorageT> pool_;
    using Builder = UnRealizedPnlStorageBuilder<Price, Qty, MemoryPool,
                                                UnRealizedPnlStorageT>;
    UnrealizedPnlCalculatorContainerT unrealized_pnl_calculator_container;

  public:
    explicit UnRealizedPnlStorageContainer(size_t size)
        : pool_(size), unrealized_pnl_calculator_container(size) {}

    auto Build() {
        // Получаем указатель на калькулятор
        auto ptr = unrealized_pnl_calculator_container.Build();

        // Создаем и возвращаем объект
        return Builder(pool_).SetPnlUnrealizedCalculator(ptr).Build();
    }
};

};  // namespace impl
};  // namespace aos