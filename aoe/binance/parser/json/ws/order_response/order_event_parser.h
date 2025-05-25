#pragma once
#include <unordered_map>

#include "aoe/binance/order_event/order_event.h"
#include "aoe/binance/parser/json/ws/order_response/i_order_event_parser.h"
#include "aos/converters/big_string_view_to_trading_pair/big_string_view_to_trading_pair.h"

namespace aoe {
namespace binance {
namespace impl {
template <template <typename> typename MemoryPool>
class OrderEventParser : public OrderEventParserInterface<MemoryPool> {
    using EventPtr  = OrderEventParserInterface<MemoryPool>::EventPtr;
    using Key       = std::string_view;
    using FactoryFn = std::function<EventPtr()>;

    struct PairHash {
        std::size_t operator()(const Key& k) const {
            return std::hash<std::string_view>{}(k);
        }
    };
    aos::impl::BigStringViewToTradingPair trading_pair_factory_;
    std::unordered_map<Key, FactoryFn, PairHash> factory_map_;
    MemoryPool<OrderEventNew<MemoryPool>> pool_order_new_;
    MemoryPool<OrderEventPendingNew<MemoryPool>> pool_order_pending_new_;
    MemoryPool<OrderEventPartiallyFilled<MemoryPool>>
        pool_order_partially_filled_;
    MemoryPool<OrderEventFilled<MemoryPool>> pool_order_filled_;
    MemoryPool<OrderEventCancelled<MemoryPool>> pool_order_cancelled_;
    MemoryPool<OrderEventPendingCancel<MemoryPool>> pool_order_pending_cancel_;
    MemoryPool<OrderEventRejected<MemoryPool>> pool_order_rejected_;
    MemoryPool<OrderEventExpired<MemoryPool>> pool_order_expired_;
    MemoryPool<OrderEventExpiredInMatch<MemoryPool>>
        pool_order_expired_in_match_;

  public:
    ~OrderEventParser() override = default;
    OrderEventParser(std::size_t pool_size)
        : pool_order_new_(pool_size),
          pool_order_pending_new_(pool_size),
          pool_order_partially_filled_(pool_size),
          pool_order_filled_(pool_size),
          pool_order_cancelled_(pool_size),
          pool_order_pending_cancel_(pool_size),
          pool_order_rejected_(pool_size),
          pool_order_expired_(pool_size),
          pool_order_expired_in_match_(pool_size) {}

    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) override {
        auto event_type = doc["e"].get_string();
        if (event_type.error() != simdjson::SUCCESS ||
            event_type.value() != "executionReport")
            return {false, nullptr};

        auto symbol = doc["s"].get_string();
        if (symbol.error() != simdjson::SUCCESS) return {false, nullptr};

        // client order id
        auto order_id = doc["c"].get_uint64();
        if (order_id.error() != simdjson::SUCCESS) return {false, nullptr};

        // X - current status order
        auto order_status = doc["X"].get_string();
        if (order_status.error() != simdjson::SUCCESS) return {false, nullptr};

        auto order_qty = doc["q"].get_double_in_string();
        if (order_qty.error() != simdjson::SUCCESS) return {false, nullptr};

        auto cum_exec_qty = doc["z"].get_double_in_string();
        if (cum_exec_qty.error() != simdjson::SUCCESS) return {false, nullptr};

        auto price = doc["p"].get_double_in_string();
        if (price.error() != simdjson::SUCCESS) return {false, nullptr};

        auto it = factory_map_.find(order_status.value());
        if (it == factory_map_.end()) return {false, nullptr};

        auto ptr = it->second();
        ptr->SetOrderId(order_id.value());
        ptr->SetPrice(price.value());
        ptr->SetOrderQty(order_qty.value());
        ptr->SetCumExecQty(cum_exec_qty.value());

        auto [status, trading_pair] =
            trading_pair_factory_.Convert(symbol.value());
        if (!status) return {false, nullptr};

        ptr->SetTradingPair(trading_pair);

        return {true, ptr};
    }

  private:
    void RegisterFromConfig() {
        factory_map_["NEW"] = [this]() {
            auto ptr = pool_order_new_.Allocate();
            ptr->SetMemoryPool(&pool_order_new_);
            return ptr;
        };
        factory_map_["PENDING_NEW"] = [this]() {
            auto ptr = pool_order_pending_new_.Allocate();
            ptr->SetMemoryPool(&pool_order_pending_new_);
            return ptr;
        };
        factory_map_["PARTIALLY_FILLED"] = [this]() {
            auto ptr = pool_order_partially_filled_.Allocate();
            ptr->SetMemoryPool(&pool_order_partially_filled_);
            return ptr;
        };
        factory_map_["FILLED"] = [this]() {
            auto ptr = pool_order_filled_.Allocate();
            ptr->SetMemoryPool(&pool_order_filled_);
            return ptr;
        };
        factory_map_["CANCELED"] = [this]() {
            auto ptr = pool_order_cancelled_.Allocate();
            ptr->SetMemoryPool(&pool_order_cancelled_);
            return ptr;
        };
        factory_map_["PENDING_CANCEL"] = [this]() {
            auto ptr = pool_order_pending_cancel_.Allocate();
            ptr->SetMemoryPool(&pool_order_pending_cancel_);
            return ptr;
        };
        factory_map_["REJECTED"] = [this]() {
            auto ptr = pool_order_rejected_.Allocate();
            ptr->SetMemoryPool(&pool_order_rejected_);
            return ptr;
        };
        factory_map_["EXPIRED"] = [this]() {
            auto ptr = pool_order_expired_.Allocate();
            ptr->SetMemoryPool(&pool_order_expired_);
            return ptr;
        };
        factory_map_["EXPIRED_IN_MATCH"] = [this]() {
            auto ptr = pool_order_expired_in_match_.Allocate();
            ptr->SetMemoryPool(&pool_order_expired_in_match_);
            return ptr;
        };
    }
};
};  // namespace impl
};  // namespace binance
};  // namespace aoe