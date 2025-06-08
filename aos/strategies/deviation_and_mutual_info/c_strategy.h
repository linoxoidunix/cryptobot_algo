// strategy_builder.h
#pragma once

#include <functional>
#include <list>

#include "aos/market_triplet_manager/i_market_triplet_manager.h"
#include "aos/sliding_window_storage/c_sliding_window_storage.h"
#include "aos/strategy/i_strategy.h"
#include "aos/strategy/strategy.h"

namespace aos {
namespace strategies {
namespace deviation_and_mutual_information {
template <typename HashT, typename T>
class Strategy : public StrategyInterface<HashT, T>{
    impl::Strategy<HashT, T> core_strategy_;
    impl::SlidingWindowStorageAvgDevMinMax<HashT, T> sliding_window_;
    impl::MutualInformationCalculator<HashT, T> mi_calculator_;
    MarketTripletManagerInterface<HashT>& market_triplet_manager_;

  private:
    void InitBuyActions() {
        core_strategy_.AddActionsToBuy(
            [](std::queue<size_t>& queue, size_t hash, const double& value) {
                if (hash == 1) {
                    logi("start analise hash:{} value:{}", hash, value);
                    queue.push(hash);
                }
            });

        core_strategy_.AddActionsToBuy([this](std::queue<size_t>& queue, size_t hash,
                                        const double& value) {
            auto [_, ratio] = sliding_window_.GetDeviationRatio(hash, value);
            auto dev        = sliding_window_.GetDeviation(hash, value);
            logi("Buy: hash:{} value:{} dev:{} ratio:{}", hash, value, dev,
                 ratio);
            if (ratio > 0.001) {
                queue.push(hash);
            }
        });

        core_strategy_.AddActionsToBuy([this](std::queue<size_t>& queue, size_t hash,
                                        const double& value) {
            if (!market_triplet_manager_.HasPair(hash)) return;
            for (const auto& pair : market_triplet_manager_.GetPairs(hash)) {
                auto [status, mi] = mi_calculator_.ComputeMutualInformation(
                    sliding_window_, hash, pair, 10);
                if (status && mi > 2) {
                    logi("Buy MI ({} <-> {}): {}", hash, pair, mi);
                    queue.push(pair);
                }
            }
        });

        core_strategy_.AddActionsToBuy(
            [](std::queue<size_t>& queue, size_t hash, const double& value) {
                logi("Need buy hash:{} value:{}", hash, value);
            });
    };
    void InitSellActions() {
        core_strategy_.AddActionsToSell(
            [](std::queue<size_t>& queue, size_t hash, const double& value) {
                if (hash == 1) {
                    logi("start analise hash:{} value:{}", hash, value);
                    queue.push(hash);
                }
            });

        core_strategy_.AddActionsToSell([this](std::queue<size_t>& queue, size_t hash,
                                         const double& value) {
            auto [_, ratio] = sliding_window_.GetDeviationRatio(hash, value);
            auto dev        = sliding_window_.GetDeviation(hash, value);
            logi("Sell: hash:{} value:{} dev:{} ratio:{}", hash, value, dev,
                 ratio);
            if (ratio < 0.001) {
                queue.push(hash);
            }
        });

        core_strategy_.AddActionsToSell([this](std::queue<size_t>& queue, size_t hash,
                                         const double& value) {
            if (!market_triplet_manager_.HasPair(hash)) return;
            for (const auto& pair : market_triplet_manager_.GetPairs(hash)) {
                auto [status, mi] = mi_calculator_.ComputeMutualInformation(
                    sliding_window_, hash, pair, 10);
                if (status && mi > 2) {
                    logi("Sell MI ({} <-> {}): {}", hash, pair, mi);
                    queue.push(pair);
                }
            }
        });

        core_strategy_.AddActionsToSell(
            [](std::queue<size_t>& queue, size_t hash, const double& value) {
                logi("Need sell hash:{} value:{}", hash, value);
            });
    }

  public:
    Strategy(int window_size, MarketTripletManagerInterface<HashT>& market_triplet_manager) :
     sliding_window_(window_size),
     market_triplet_manager_(market_triplet_manager) {
        InitBuyActions();
        InitSellActions();
    }
    void AnalyzeToBuy(const HashT asset, const T& value) override {
        core_strategy_.AnalyzeToBuy(asset, value);
    }

    void AnalyzeToSell(const HashT asset, const T& value) override {
        core_strategy_.AnalyzeToSell(asset, value);
    }
    void AddData(const HashT hash_asset, const T& value) override{
        sliding_window_.AddData(hash_asset, value);
    };
    ~Strategy() override = default;
};
};  // namespace deviation_and_mutual_information
};  // namespace strategies
};  // namespace aos