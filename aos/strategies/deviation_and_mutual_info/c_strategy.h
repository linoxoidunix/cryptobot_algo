// strategy_builder.h
#pragma once

#include <functional>
#include <list>

#include "aos/hash_utils/decompose_hash.h"
#include "aos/logger/mylog.h"
#include "aos/market_triplet_manager/i_market_triplet_manager.h"
#include "aos/mutual_information/c_mutual_information_calculator.h"
#include "aos/sliding_window_storage/c_sliding_window_storage.h"
#include "aos/strategies/deviation_and_mutual_info/config.h"
#include "aos/strategy/i_strategy.h"
#include "aos/strategy/strategy.h"

namespace aos {
namespace strategies {
namespace deviation_and_mutual_information {
template <typename HashT, typename T>
class Strategy : public StrategyInterface<HashT, T> {
    impl::Strategy<HashT, T> core_strategy_;
    impl::SlidingWindowStorageAvgDevMinMax<HashT, T> sliding_window_;
    impl::MutualInformationCalculator<HashT, T> mi_calculator_;
    MarketTripletManagerInterface<HashT>& market_triplet_manager_;
    const Config<HashT>& config_;

  private:
    void InitBuyActions() {
        core_strategy_.AddActionsToBuy(
            [this](std::queue<HashT>& queue, HashT hash, const T& value) {
                if (hash == config_.allowed_asset_hash) {
                    logi("start analise hash:{} value:{}", hash, value);
                    queue.push(hash);
                }
            });

        core_strategy_.AddActionsToBuy([this](std::queue<HashT>& queue,
                                              HashT hash, const T& value) {
            auto [_, ratio] = sliding_window_.GetDeviationRatio(hash, value);
            auto [status_deviation, dev] =
                sliding_window_.GetDeviation(hash, value);
            logi("Buy: hash:{} value:{} dev:{} ratio:{}", hash, value, dev,
                 ratio);
            if (ratio > config_.deviation_ratio_threshold) {
                queue.push(hash);
            }
        });

        core_strategy_.AddActionsToBuy([this](std::queue<HashT>& queue,
                                              HashT hash, const T& value) {
            if (!market_triplet_manager_.HasPair(hash)) return;
            for (const auto& pair : market_triplet_manager_.GetPairs(hash)) {
                auto [status, mi] = mi_calculator_.ComputeMutualInformation(
                    sliding_window_, hash, pair, config_.number_bins);
                logi("Buy status:{} mi:{}", status, mi);
                if (status && mi > config_.mi_threshold) {
                    logi("Buy MI ({} <-> {}): {}", hash, pair, mi);
                    queue.push(pair);
                }
            }
        });

        core_strategy_.AddActionsToBuy([](std::queue<HashT>& queue, HashT hash,
                                          const T& value) {
            common::ExchangeId exchange_id_1 = common::ExchangeId::kInvalid;
            aos::NetworkEnvironment network_environment_1;
            aos::CategoryRaw category_market_1;
            aos::TradingPair trading_pair_1;
            aos::DecomposeHash(hash, exchange_id_1, category_market_1,
                               network_environment_1, trading_pair_1);
            logi("need buy with hash={} with value={} on {} category={} {} {}",
                 hash, value, exchange_id_1, category_market_1,
                 network_environment_1, trading_pair_1);
        });
    };
    void InitSellActions() {
        core_strategy_.AddActionsToSell(
            [this](std::queue<HashT>& queue, HashT hash, const T& value) {
                if (hash == config_.allowed_asset_hash) {
                    logi("start analise hash:{} value:{}", hash, value);
                    queue.push(hash);
                }
            });

        core_strategy_.AddActionsToSell([this](std::queue<HashT>& queue,
                                               HashT hash, const T& value) {
            auto [_, ratio] = sliding_window_.GetDeviationRatio(hash, value);
            auto [status_deviation, dev] =
                sliding_window_.GetDeviation(hash, value);
            logi("Sell: hash:{} value:{} dev:{} ratio:{}", hash, value, dev,
                 ratio);
            if (ratio > config_.deviation_ratio_threshold) {
                queue.push(hash);
            }
        });

        core_strategy_.AddActionsToSell([this](std::queue<HashT>& queue,
                                               HashT hash, const T& value) {
            if (!market_triplet_manager_.HasPair(hash)) return;
            for (const auto& pair : market_triplet_manager_.GetPairs(hash)) {
                auto [status, mi] = mi_calculator_.ComputeMutualInformation(
                    sliding_window_, hash, pair, config_.number_bins);
                logi("Sell status:{} mi:{}", status, mi);
                if (status && mi > config_.mi_threshold) {
                    logi("Sell MI ({} <-> {}): {}", hash, pair, mi);
                    queue.push(pair);
                }
            }
        });

        core_strategy_.AddActionsToSell([](std::queue<HashT>& queue, HashT hash,
                                           const T& value) {
            common::ExchangeId exchange_id_1 = common::ExchangeId::kInvalid;
            aos::NetworkEnvironment network_environment_1;
            aos::CategoryRaw category_market_1;
            aos::TradingPair trading_pair_1;
            aos::DecomposeHash(hash, exchange_id_1, category_market_1,
                               network_environment_1, trading_pair_1);
            logi("need sell with hash={} with value={} on {} category={} {} {}",
                 hash, value, exchange_id_1, category_market_1,
                 network_environment_1, trading_pair_1);
        });
    }

  public:
    Strategy(MarketTripletManagerInterface<HashT>& market_triplet_manager,
             const Config<HashT>& config)
        : sliding_window_(config.window_size),
          market_triplet_manager_(market_triplet_manager),
          config_(config) {}
    void Init() {
        InitBuyActions();
        InitSellActions();
    }

    void AnalyzeToBuy(const HashT asset, const T& value) override {
        core_strategy_.AnalyzeToBuy(asset, value);
    }

    void AnalyzeToSell(const HashT asset, const T& value) override {
        core_strategy_.AnalyzeToSell(asset, value);
    }
    void AddData(const HashT hash_asset, const T& value) override {
        sliding_window_.AddData(hash_asset, value);
    };
    ~Strategy() override = default;
};
};  // namespace deviation_and_mutual_information
};  // namespace strategies
};  // namespace aos