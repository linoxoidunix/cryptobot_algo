#pragma once
#include <functional>
#include <list>
#include <unordered_map>
#include <vector>

#include "aoe/binance/order_book_event/order_book_event.h"
#include "aoe/binance/parser/json/rest/snapshot_response/i_parser.h"
#include "aos/converters/big_string_view_to_trading_pair/big_string_view_to_trading_pair.h"
#include "aos/order_book_level_raw/order_book_level_raw.h"

namespace aoe {
namespace binance {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class SnapshotEventParser
    : public SnapshotEventParserInterface<Price, Qty, MemoryPool> {
    using EventPtr =
        SnapshotEventParserInterface<Price, Qty, MemoryPool>::EventPtr;
    using Key       = std::string_view;
    using FactoryFn = std::function<EventPtr()>;

    struct PairHash {
        std::size_t operator()(const Key& k) const {
            return std::hash<std::string_view>{}(k);
        }
    };
    aos::impl::BigStringViewToTradingPair trading_pair_factory_;
    FactoryFn factory_event_;
    MemoryPool<OrderBookSnapshotEventDefault<Price, Qty, MemoryPool>>
        pool_order_snapshot_;

  public:
    ~SnapshotEventParser() override {}
    SnapshotEventParser(std::size_t pool_size)
        : pool_order_snapshot_(pool_size) {
        RegisterFromConfig();
    }

    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) override {
        EventPtr ptr          = factory_event_();
        // update_id
        auto update_id_result = doc["lastUpdateId"].get_uint64();
        if (update_id_result.error() != simdjson::SUCCESS)
            return {false, nullptr};
        ptr->SetUpdateId(update_id_result.value());

        // auto [status, trading_pair] =
        //     trading_pair_factory_.Convert(symbol_result.value());
        // if (!status) return {false, nullptr};
        // ptr->SetTradingPair(trading_pair);

        // bids
        auto bids_result = doc["bids"].get_array();
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
        auto asks_result = doc["asks"].get_array();
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
        factory_event_ = [this]() {
            auto ptr = pool_order_snapshot_.Allocate();
            ptr->SetMemoryPool(&pool_order_snapshot_);
            return ptr;
        };
    }
};
};  // namespace impl
};  // namespace binance
};  // namespace aoe