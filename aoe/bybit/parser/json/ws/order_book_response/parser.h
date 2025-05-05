#pragma once
#include <functional>
#include <list>
#include <unordered_map>
#include <vector>

#include "aoe/bybit/order_book_event/order_book_event.h"
#include "aoe/bybit/parser/json/ws/order_book_response/i_parser.h"
#include "aos/order_book_level_raw/order_book_level_raw.h"
#include "aos/trading_pair_factory/i_trading_pair_factory.h"
namespace aoe {
namespace bybit {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class OrderBookEventParser
    : public OrderBookEventParserInterface<Price, Qty, MemoryPool> {
    using EventPtr =
        OrderBookEventParserInterface<Price, Qty, MemoryPool>::EventPtr;
    using Key       = std::string_view;
    using FactoryFn = std::function<EventPtr()>;

    struct PairHash {
        std::size_t operator()(const Key& k) const {
            return std::hash<std::string_view>{}(k);
        }
    };
    aos::TradingPairFactoryInterface& trading_pair_factory_;
    std::unordered_map<Key, FactoryFn, PairHash> factory_map_;
    MemoryPool<OrderBookSnapshotEventDefault<Price, Qty, MemoryPool>>
        pool_order_snapshot_;
    MemoryPool<OrderBookDiffEventDefault<Price, Qty, MemoryPool>>
        pool_order_diff_;

  public:
    ~OrderBookEventParser() override {}
    OrderBookEventParser(std::size_t pool_size,
                         aos::TradingPairFactoryInterface& trading_pair_factory)
        : trading_pair_factory_(trading_pair_factory),
          pool_order_snapshot_(pool_size),
          pool_order_diff_(pool_size) {}

    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) override {
        auto data_json    = doc["data"];
        auto array_result = data_json.get_array();
        if (array_result.error() != simdjson::SUCCESS) return {false, nullptr};
        for (auto item : array_result.value()) {
            auto update_id = item["u"].get_uint64_in_string();
            if (update_id.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);

            auto symbol = item["s"];
            if (symbol.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto it = factory_map_.find(symbol);

            if (it != factory_map_.end()) {
                auto ptr = it->second();
                ptr->SetUpdateId(update_id.value());
                auto [status, trading_pair] =
                    trading_pair_factory_.Produce(symbol.value());
                if (!status) return std::make_pair(false, nullptr);
                ptr->SetTradingPair(trading_pair);

                auto bids_array_result = item["b"].get_array();
                if (bids_array_result.error() == simdjson::SUCCESS) {
                    std::vector<aos::OrderBookLevelRaw<Price, Qty>> bids;
                    // i knew that max 500 lvl for bybit
                    bids.reserve(1000);
                    for (auto bid_entry : bids_array_result.value()) {
                        simdjson::ondemand::array bid_pair =
                            bid_entry.get_array();
                        auto price_result =
                            bid_pair.at(0).get_double_in_string();
                        if (price_result.error() != simdjson::SUCCESS) {
                            return std::make_pair(false, nullptr);
                        }
                        auto qty_result = bid_pair.at(1).get_double_in_string();
                        if (qty_result.error() != simdjson::SUCCESS) {
                            return std::make_pair(false, nullptr);
                        }
                        bids.emplace_back(price_result.value(),
                                          qty_result.value());
                    }
                    ptr->SetBids(std::move(bids));
                }
                auto asks_array_result = item["a"].get_array();
                if (asks_array_result.error() == simdjson::SUCCESS) {
                    std::vector<aos::OrderBookLevelRaw<Price, Qty>> asks;
                    // i knew that max 500 lvl for bybit
                    asks.reserve(1000);
                    for (auto ask_entry : asks_array_result.value()) {
                        simdjson::ondemand::array ask_pair =
                            ask_entry.get_array();
                        auto price_result =
                            ask_pair.at(0).get_double_in_string();
                        if (price_result.error() != simdjson::SUCCESS) {
                            return std::make_pair(false, nullptr);
                        }
                        auto qty_result = ask_pair.at(1).get_double_in_string();
                        if (qty_result.error() != simdjson::SUCCESS) {
                            return std::make_pair(false, nullptr);
                        }
                        asks.emplace_back(price_result.value(),
                                          qty_result.value());
                    }
                    ptr->SetAsks(std::move(asks));
                }
                return std::make_pair(true, ptr);
            }
        }
        return {false, nullptr};
    }

  private:
    void RegisterFromConfig() {
        factory_map_["snapshot"] = [this]() {
            auto ptr = pool_order_snapshot_.Allocate();
            ptr->SetMemoryPool(&pool_order_snapshot_);
            return ptr;
        };
        factory_map_["delta"] = [this]() {
            auto ptr = pool_order_diff_.Allocate();
            ptr->SetMemoryPool(&pool_order_diff_);
            return ptr;
        };
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe