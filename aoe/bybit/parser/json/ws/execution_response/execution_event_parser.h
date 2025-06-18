#pragma once
#include <vector>

#include "aoe/bybit/execution_event/types.h"
#include "aoe/bybit/parser/json/ws/execution_response/i_execution_event_parser.h"
#include "aos/common/multi_pool.h"
#include "aos/converters/big_string_view_to_trading_pair/big_string_view_to_trading_pair.h"
#include "aos/trading_pair_factory/i_trading_pair_factory.h"
#include "boost/intrusive_ptr.hpp"
#include "simdjson.h"  // NOLINT
namespace aoe {
namespace bybit {
namespace impl {
template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventParser
    : public ExecutionEventParserInterface<MemoryPool, PositionT> {
    using EventPtr =
        boost::intrusive_ptr<ExecutionEventInterface<MemoryPool, PositionT>>;
    using Key       = std::pair<std::string_view, std::string_view>;
    using FactoryFn = std::function<EventPtr()>;

    struct PairHash {
        std::size_t operator()(const Key& k) const {
            return std::hash<std::string_view>{}(k.first) ^
                   std::hash<std::string_view>{}(k.second);
        }
    };
    aos::impl::BigStringViewToTradingPair trading_pair_factory_;
    std::unordered_map<Key, FactoryFn, PairHash> factory_map_;

    MemoryPool<ExecutionEventSpotSellDefault<MemoryPool, PositionT>>
        pool_spot_sell_{200};
    MemoryPool<ExecutionEventSpotBuyDefault<MemoryPool, PositionT>>
        pool_spot_buy_{200};
    MemoryPool<ExecutionEventLinearSellDefault<MemoryPool, PositionT>>
        pool_linear_sell_{200};
    MemoryPool<ExecutionEventLinearBuyDefault<MemoryPool, PositionT>>
        pool_linear_buy_{200};

  public:
    ~ExecutionEventParser() override = default;
    explicit ExecutionEventParser(std::size_t pool_size)
        : pool_spot_sell_(pool_size),
          pool_spot_buy_(pool_size),
          pool_linear_sell_(pool_size),
          pool_linear_buy_(pool_size) {
        RegisterFromConfig();
    }
    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) {
        auto data_json    = doc["data"];
        auto array_result = data_json.get_array();
        if (array_result.error() != simdjson::SUCCESS) return {false, nullptr};
        for (auto item : array_result.value()) {
            auto category = item["category"];
            if (category.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto side = item["side"];
            if (side.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto fee_rate = item["execFee"].get_double_in_string();
            if (fee_rate.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto exec_price = item["execPrice"].get_double_in_string();
            if (exec_price.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto exec_qty = item["execQty"].get_double_in_string();
            if (exec_qty.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto exec_value = item["execValue"].get_double_in_string();
            if (exec_value.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto symbol = item["symbol"];
            if (symbol.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto order_id = item["orderId"].get_uint64_in_string();
            if (order_id.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto it = factory_map_.find({category.value(), side.value()});

            if (it != factory_map_.end()) {
                auto ptr = it->second();
                ptr->SetFee(fee_rate.value());
                ptr->SetExecPrice(exec_price.value());
                ptr->SetExecQty(exec_qty.value());
                ptr->SetExecValue(exec_value.value());
                ptr->SetOrderId(order_id.value());
                auto [status, trading_pair] =
                    trading_pair_factory_.Convert(symbol.value());
                if (!status) return std::make_pair(false, nullptr);
                ptr->SetTradingPair(trading_pair);

                return std::make_pair(true, ptr);
            }
        }
        return {false, nullptr};
    }

    void RegisterFromConfig() {
        factory_map_[{"spot", "Buy"}] = [this]() {
            auto ptr = pool_spot_buy_.Allocate();
            ptr->SetMemoryPool(&pool_spot_buy_);
            return ptr;
        };
        factory_map_[{"spot", "Sell"}] = [this]() {
            auto ptr = pool_spot_sell_.Allocate();
            ptr->SetMemoryPool(&pool_spot_sell_);
            return ptr;
        };
        factory_map_[{"linear", "Buy"}] = [this]() {
            auto ptr = pool_linear_buy_.Allocate();
            ptr->SetMemoryPool(&pool_linear_buy_);
            return ptr;
        };
        factory_map_[{"linear", "Sell"}] = [this]() {
            auto ptr = pool_linear_sell_.Allocate();
            ptr->SetMemoryPool(&pool_linear_sell_);
            return ptr;
        };
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe