#pragma once
#include <functional>
#include <list>
#include <unordered_map>
#include <vector>

#include "aoe/binance/order_book_event/order_book_event.h"
#include "aoe/binance/parser/json/ws/diff_response/i_parser.h"
#include "aos/converters/big_string_view_to_trading_pair/big_string_view_to_trading_pair.h"
#include "aos/order_book_level_raw/order_book_level_raw.h"

namespace aoe {
namespace binance {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class DiffEventParser
    : public DiffEventParserInterface<Price, Qty, MemoryPool> {
    using EventPtr = DiffEventParserInterface<Price, Qty, MemoryPool>::EventPtr;
    using Key      = std::string_view;
    using FactoryFn = std::function<EventPtr()>;

    struct PairHash {
        std::size_t operator()(const Key& k) const {
            return std::hash<std::string_view>{}(k);
        }
    };
    aos::impl::BigStringViewToTradingPair trading_pair_factory_;
    FactoryFn factory_event_;
    MemoryPool<OrderBookDiffEventDefault<Price, Qty, MemoryPool>>
        pool_order_diff_;

  public:
    ~DiffEventParser() override {}
    DiffEventParser(std::size_t pool_size) : pool_order_diff_(pool_size) {
        RegisterFromConfig();
    }

    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) override {
        std::string_view event_type;
        auto status_type_event = doc["e"].get_string().get(event_type);
        if (status_type_event != simdjson::SUCCESS) return {false, nullptr};
        if (event_type != "depthUpdate") return {false, nullptr};

        // simdjson::simdjson_result<simdjson::ondemand::object> data_obj_result
        // =
        //     doc["data"].get_object();
        // if (data_obj_result.error() != simdjson::SUCCESS)
        //     return {false, nullptr};

        // simdjson::ondemand::object doc = data_obj_result.value();

        // auto it_factory                     = factory_map_.find(event_type);
        // if (it_factory == factory_map_.end()) return {false, nullptr};

        EventPtr ptr                = factory_event_();

        // update final id
        auto update_final_id_result = doc["u"].get_uint64();
        if (update_final_id_result.error() != simdjson::SUCCESS)
            return {false, nullptr};
        ptr->SetFinalUpdateId(update_final_id_result.value());

        // update first id
        auto update_first_id_result = doc["U"].get_uint64();
        if (update_first_id_result.error() != simdjson::SUCCESS)
            return {false, nullptr};
        ptr->SetFirstUpdateId(update_first_id_result.value());

        // symbol
        simdjson::simdjson_result<std::string_view> symbol_result =
            doc["s"].get_string();
        if (symbol_result.error() != simdjson::SUCCESS) return {false, nullptr};

        auto [status, trading_pair] =
            trading_pair_factory_.Convert(symbol_result.value());
        if (!status) return {false, nullptr};
        ptr->SetTradingPair(trading_pair);
        logd("raw={} {}", symbol_result.value(), trading_pair);
        // bids
        auto bids_result = doc["b"].get_array();
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
        auto asks_result = doc["a"].get_array();
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
        // return {false, nullptr};
    }

  private:
    void RegisterFromConfig() {
        factory_event_ = [this]() {
            auto ptr = pool_order_diff_.Allocate();
            ptr->SetMemoryPool(&pool_order_diff_);
            return ptr;
        };
    }
};
};  // namespace impl
};  // namespace binance
};  // namespace aoe