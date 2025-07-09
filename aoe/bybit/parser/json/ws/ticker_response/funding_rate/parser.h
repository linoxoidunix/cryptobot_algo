#pragma once
#include <functional>
#include <list>
#include <unordered_map>
#include <vector>

#include "aoe/bybit/event/funding_rate_event/funding_rate_event.h"
#include "aoe/bybit/parser/json/ws/ticker_response/funding_rate/i_parser.h"
#include "aos/converters/big_string_view_to_trading_pair/big_string_view_to_trading_pair.h"

namespace aoe {
namespace bybit {
namespace impl {
template <template <typename> typename MemoryPool>
class FundingRateEventParser
    : public FundingRateEventParserInterface<MemoryPool> {
    using EventPtr  = FundingRateEventParserInterface<MemoryPool>::EventPtr;
    using Key       = std::string_view;
    using FactoryFn = std::function<EventPtr()>;

    struct PairHash {
        std::size_t operator()(const Key& k) const {
            return std::hash<std::string_view>{}(k);
        }
    };
    std::unordered_map<Key, FactoryFn, PairHash> factory_map_;

    aos::impl::BigStringViewToTradingPair trading_pair_factory_;
    MemoryPool<FundingRateEvent<MemoryPool>> pool_funding_rate_snapshot_;

  public:
    ~FundingRateEventParser() override = default;
    explicit FundingRateEventParser(std::size_t pool_size)
        : pool_funding_rate_snapshot_(pool_size) {
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

        EventPtr ptr = it_factory->second();

        // update next_funding_time
        auto next_funding_time_result =
            data_obj["nextFundingTime"].get_uint64_in_string();
        if (next_funding_time_result.error() != simdjson::SUCCESS)
            return {false, nullptr};
        ptr->SetNextFundingTime(next_funding_time_result.value());

        // update funding_rate
        auto funding_rate = data_obj["fundingRate"].get_double_in_string();
        if (funding_rate.error() != simdjson::SUCCESS) return {false, nullptr};
        ptr->SetFundingRate(funding_rate.value());

        // symbol
        simdjson::simdjson_result<std::string_view> symbol_result =
            data_obj["symbol"].get_string();
        if (symbol_result.error() != simdjson::SUCCESS) return {false, nullptr};

        auto [status, trading_pair] =
            aos::impl::BigStringViewToTradingPair::Convert(
                symbol_result.value());
        if (!status) return {false, nullptr};
        ptr->SetTradingPair(trading_pair);
        return {true, ptr};
    }

  private:
    void RegisterFromConfig() {
        factory_map_["snapshot"] = [this]() {
            auto ptr = pool_funding_rate_snapshot_.Allocate();
            ptr->SetMemoryPool(&pool_funding_rate_snapshot_);
            return ptr;
        };
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe