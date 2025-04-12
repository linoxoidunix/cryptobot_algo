#pragma once
#include <functional>
#include <list>

#include "aos/strategy/i_strategy.h"

namespace aos {
namespace impl {
template <typename HashT, typename T>
class Strategy {
    std::list<std::function<void(std::queue<HashT>& queue_to_buy,
                                 const HashT hasht, const T& value)>>
        actions_to_buy_;
    std::list<std::function<void(std::queue<HashT>& queue_to_sell,
                                 const HashT hasht, const T& value)>>
        actions_to_sell_;

  public:
    Strategy() = default;
    void AddActionsToBuy(std::function<void(std::queue<HashT>& queue_to_buy,
                                            const HashT hasht, const T& value)>
                             observer) override {
        actions_to_buy_.push_back(observer);
    }
    void AddActionsToSell(std::function<void(std::queue<HashT>& queue_to_buy,
                                             const HashT hasht, const T& value)>
                              observer) override {
        actions_to_sell_.push_back(observer);
    }
    void AnalyzeToBuy(const HashT asset, const T& value) {
        std::queue<HashT> assets_to_buy;
        assets_to_buy.push(asset);
        auto begin = actions_to_buy_.begin();
        while (!assets_to_buy.empty()) {
            if (begin == actions_to_buy_.end()) {
                logi("no registered cb found");
                break;
            }
            size_t size = assets_to_buy.size();
            for (size_t i = 0; i < size; i++) {
                auto front_hash = assets_to_buy.front();
                (*begin)(assets_to_buy, front_hash, value);
                assets_to_buy.pop();
            }
            begin = std::next(begin);
        }
    }

    void AnalyzeToSell(const HashT asset, const T& value) {
        std::queue<HashT> assets_to_sell;
        assets_to_sell.push(asset);
        auto begin = actions_to_sell_.begin();
        while (!assets_to_sell.empty()) {
            if (begin == actions_to_sell_.end()) {
                logi("no registered cb found");
                break;
            }
            size_t size = assets_to_sell.size();
            for (size_t i = 0; i < size; i++) {
                auto front_hash = assets_to_sell.front();
                (*begin)(assets_to_sell, front_hash, value);
                assets_to_sell.pop();
            }
            begin = std::next(begin);
        }
    }
};
};  // namespace impl
};  // namespace aos