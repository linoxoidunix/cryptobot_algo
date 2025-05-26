#pragma once
#include <vector>

#include "aoe/binance/constants/constants.h"
#include "aoe/binance/execution_event/types.h"
#include "aoe/binance/parser/json/ws/execution_response/i_execution_event_parser.h"
#include "aos/common/multi_pool.h"
#include "aos/converters/big_string_view_to_trading_pair/big_string_view_to_trading_pair.h"
#include "aos/trading_pair_factory/i_trading_pair_factory.h"
#include "boost/intrusive_ptr.hpp"
#include "simdjson.h"
namespace aoe {
namespace binance {
namespace spot {
namespace impl {
template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventParser
    : public ExecutionEventParserInterface<MemoryPool, PositionT> {
    using EventPtr =
        boost::intrusive_ptr<ExecutionEventInterface<MemoryPool, PositionT>>;
    using Key       = std::string_view;
    using FactoryFn = std::function<EventPtr()>;

    struct PairHash {
        std::size_t operator()(const Key& k) const {
            return std::hash<std::string_view>{}(k);
        }
    };
    aos::impl::BigStringViewToTradingPair trading_pair_factory_;
    std::unordered_map<Key, FactoryFn, PairHash> factory_map_;

    MemoryPool<ExecutionEventSpotSellDefault<MemoryPool, PositionT>>
        pool_spot_sell_{kInitialExecutionEventPoolSize};
    MemoryPool<ExecutionEventSpotBuyDefault<MemoryPool, PositionT>>
        pool_spot_buy_{kInitialExecutionEventPoolSize};

  public:
    ~ExecutionEventParser() override = default;
    explicit ExecutionEventParser(std::size_t pool_size)
        : pool_spot_sell_(pool_size), pool_spot_buy_(pool_size) {
        RegisterFromConfig();
    }
    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) {
        // Тип исполнения может быть NEW, TRADE, FILLED и т.д.
        auto exec_type = doc["x"];
        if (exec_type.error() != simdjson::SUCCESS ||
            exec_type.value() != "TRADE") {
            return {false, nullptr};  // мы обрабатываем только исполнения
        }

        auto symbol = doc["s"];
        if (symbol.error() != simdjson::SUCCESS) return {false, nullptr};

        auto side = doc["S"];
        if (side.error() != simdjson::SUCCESS) return {false, nullptr};

        auto fee = doc["n"].get_double_in_string();
        if (fee.error() != simdjson::SUCCESS) return {false, nullptr};

        auto exec_price = doc["L"].get_double_in_string();
        if (exec_price.error() != simdjson::SUCCESS) return {false, nullptr};

        auto exec_qty = doc["l"].get_double_in_string();
        if (exec_qty.error() != simdjson::SUCCESS) return {false, nullptr};

        auto exec_value = doc["Y"].get_double_in_string();  // quote asset value
        if (exec_value.error() != simdjson::SUCCESS) return {false, nullptr};

        // client order id
        auto order_id = doc["c"].get_uint64_in_string();
        if (order_id.error() != simdjson::SUCCESS) return {false, nullptr};

        auto it = factory_map_.find(side.value());
        if (it == factory_map_.end()) return {false, nullptr};

        auto ptr = it->second();
        ptr->SetFee(fee.value());
        ptr->SetExecPrice(exec_price.value());
        ptr->SetExecQty(exec_qty.value());
        ptr->SetExecValue(exec_value.value());
        ptr->SetOrderId(order_id.value());

        auto [status, trading_pair] =
            trading_pair_factory_.Convert(symbol.value());
        if (!status) return {false, nullptr};
        ptr->SetTradingPair(trading_pair);

        return {true, ptr};
    }

    void RegisterFromConfig() {
        factory_map_["BUY"] = [this]() {
            auto ptr = pool_spot_buy_.Allocate();
            ptr->SetMemoryPool(&pool_spot_buy_);
            return ptr;
        };
        factory_map_["SELL"] = [this]() {
            auto ptr = pool_spot_sell_.Allocate();
            ptr->SetMemoryPool(&pool_spot_sell_);
            return ptr;
        };
    }
};
};  // namespace impl
};  // namespace spot
namespace futures {
namespace impl {
template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventParser
    : public ExecutionEventParserInterface<MemoryPool, PositionT> {
    using EventPtr =
        boost::intrusive_ptr<ExecutionEventInterface<MemoryPool, PositionT>>;
    using Key       = std::string_view;
    using FactoryFn = std::function<EventPtr()>;

    struct PairHash {
        std::size_t operator()(const Key& k) const {
            return std::hash<std::string_view>{}(k);
        }
    };
    aos::impl::BigStringViewToTradingPair trading_pair_factory_;
    std::unordered_map<Key, FactoryFn, PairHash> factory_map_;

    MemoryPool<ExecutionEventFuturesSellDefault<MemoryPool, PositionT>>
        pool_futures_sell_{kInitialExecutionEventPoolSize};
    MemoryPool<ExecutionEventFuturesBuyDefault<MemoryPool, PositionT>>
        pool_futures_buy_{kInitialExecutionEventPoolSize};

  public:
    ~ExecutionEventParser() override = default;
    explicit ExecutionEventParser(std::size_t pool_size)
        : pool_futures_sell_(pool_size), pool_futures_buy_(pool_size) {
        RegisterFromConfig();
    }
    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) {
        // Тип исполнения может быть NEW, TRADE, FILLED и т.д.
        auto exec_type = doc["x"];
        if (exec_type.error() != simdjson::SUCCESS ||
            exec_type.value() != "TRADE") {
            return {false, nullptr};  // мы обрабатываем только исполнения
        }

        auto symbol = doc["s"];
        if (symbol.error() != simdjson::SUCCESS) return {false, nullptr};

        auto side = doc["S"];
        if (side.error() != simdjson::SUCCESS) return {false, nullptr};

        auto fee = doc["n"].get_double_in_string();
        if (fee.error() != simdjson::SUCCESS) return {false, nullptr};

        auto exec_price = doc["L"].get_double_in_string();
        if (exec_price.error() != simdjson::SUCCESS) return {false, nullptr};

        auto exec_qty = doc["l"].get_double_in_string();
        if (exec_qty.error() != simdjson::SUCCESS) return {false, nullptr};

        auto exec_value = doc["Y"].get_double_in_string();  // quote asset value
        if (exec_value.error() != simdjson::SUCCESS) return {false, nullptr};

        // client order id
        auto order_id = doc["c"].get_uint64_in_string();
        if (order_id.error() != simdjson::SUCCESS) return {false, nullptr};

        auto it = factory_map_.find(side.value());
        if (it == factory_map_.end()) return {false, nullptr};

        auto ptr = it->second();
        ptr->SetFee(fee.value());
        ptr->SetExecPrice(exec_price.value());
        ptr->SetExecQty(exec_qty.value());
        ptr->SetExecValue(exec_value.value());
        ptr->SetOrderId(order_id.value());

        auto [status, trading_pair] =
            trading_pair_factory_.Convert(symbol.value());
        if (!status) return {false, nullptr};
        ptr->SetTradingPair(trading_pair);

        return {true, ptr};
    }

    void RegisterFromConfig() {
        factory_map_["BUY"] = [this]() {
            auto ptr = pool_futures_buy_.Allocate();
            ptr->SetMemoryPool(&pool_futures_buy_);
            return ptr;
        };
        factory_map_["SELL"] = [this]() {
            auto ptr = pool_futures_sell_.Allocate();
            ptr->SetMemoryPool(&pool_futures_sell_);
            return ptr;
        };
    }
};
};  // namespace impl
};  // namespace futures
};  // namespace binance
};  // namespace aoe