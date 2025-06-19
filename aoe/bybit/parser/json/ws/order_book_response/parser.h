#pragma once
#include <functional>
#include <list>
#include <unordered_map>
#include <vector>

#include "aoe/bybit/order_book_event/order_book_event.h"
#include "aoe/bybit/parser/json/ws/order_book_response/i_parser.h"
#include "aos/converters/big_string_view_to_trading_pair/big_string_view_to_trading_pair.h"
#include "aos/order_book_level_raw/order_book_level_raw.h"

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
    aos::impl::BigStringViewToTradingPair trading_pair_factory_;
    std::unordered_map<Key, FactoryFn, PairHash> factory_map_;
    MemoryPool<OrderBookSnapshotEventDefault<Price, Qty, MemoryPool>>
        pool_order_snapshot_;
    MemoryPool<OrderBookDiffEventDefault<Price, Qty, MemoryPool>>
        pool_order_diff_;

  public:
    ~OrderBookEventParser() override = default;
    explicit OrderBookEventParser(std::size_t pool_size)
        : pool_order_snapshot_(pool_size), pool_order_diff_(pool_size) {
        RegisterFromConfig();
    }

    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) override {
        std::string_view event_type;
        auto status_type_event = doc["type"].get_string().get(event_type);
        if (status_type_event != simdjson::SUCCESS) return {false, nullptr};

        simdjson::simdjson_result<simdjson::ondemand::object> data_obj_result =
            doc["data"].get_object();
        if (data_obj_result.error() != simdjson::SUCCESS)
            return {false, nullptr};

        simdjson::ondemand::object data_obj = data_obj_result.value();

        auto it_factory                     = factory_map_.find(event_type);
        if (it_factory == factory_map_.end()) return {false, nullptr};

        EventPtr ptr          = it_factory->second();

        // update_id
        auto update_id_result = data_obj["u"].get_uint64();
        if (update_id_result.error() != simdjson::SUCCESS)
            return {false, nullptr};
        ptr->SetUpdateId(update_id_result.value());

        // symbol
        simdjson::simdjson_result<std::string_view> symbol_result =
            data_obj["s"].get_string();
        if (symbol_result.error() != simdjson::SUCCESS) return {false, nullptr};

        auto [status, trading_pair] =
            aos::impl::BigStringViewToTradingPair::Convert(
                symbol_result.value());
        if (!status) return {false, nullptr};
        ptr->SetTradingPair(trading_pair);

        // bids
        auto bids_result = data_obj["b"].get_array();
        if (bids_result.error() == simdjson::SUCCESS) {
            std::vector<aos::OrderBookLevelRaw<Price, Qty>> bids;
            bids.reserve(1000);
            for (auto bid_entry : bids_result.value()) {
                simdjson::ondemand::array bid_pair = bid_entry.get_array();
                auto it                            = bid_pair.begin();
                auto price_result = (*it).get_double_in_string();
                if (price_result.error() != simdjson::SUCCESS)
                    return {false, nullptr};
                ++it;

                auto qty_result = (*it).get_double_in_string();
                if (qty_result.error() != simdjson::SUCCESS)
                    return {false, nullptr};
                bids.emplace_back(price_result.value(), qty_result.value());
            }
            ptr->SetBids(std::move(bids));
        }

        // asks
        auto asks_result = data_obj["a"].get_array();
        if (asks_result.error() == simdjson::SUCCESS) {
            std::vector<aos::OrderBookLevelRaw<Price, Qty>> asks;
            asks.reserve(1000);
            for (auto ask_entry : asks_result.value()) {
                simdjson::ondemand::array ask_pair = ask_entry.get_array();
                auto it                            = ask_pair.begin();
                auto price_result = (*it).get_double_in_string();
                if (price_result.error() != simdjson::SUCCESS)
                    return {false, nullptr};
                ++it;

                auto qty_result = (*it).get_double_in_string();
                if (qty_result.error() != simdjson::SUCCESS)
                    return {false, nullptr};
                asks.emplace_back(price_result.value(), qty_result.value());
            }
            ptr->SetAsks(std::move(asks));
        }

        return {true, ptr};
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