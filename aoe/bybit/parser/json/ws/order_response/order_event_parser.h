#pragma once
#include <unordered_map>

#include "aoe/bybit/order_event/order_event.h"
#include "aoe/bybit/parser/json/ws/order_response/i_order_event_parser.h"

namespace aoe {
namespace bybit {
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
    aos::TradingPairFactoryInterface& trading_pair_factory_;
    std::unordered_map<Key, FactoryFn, PairHash> factory_map_;
    MemoryPool<OrderEventNew<MemoryPool>> pool_order_new_;
    MemoryPool<OrderEventPartiallyFilled<MemoryPool>>
        pool_order_partially_filled_;
    MemoryPool<OrderEventUntriggered<MemoryPool>> pool_order_untriggered_;
    MemoryPool<OrderEventRejected<MemoryPool>> pool_order_rejected_;
    MemoryPool<OrderEventPartiallyFilledCancelled<MemoryPool>>
        pool_order_partially_filled_canceled_;
    MemoryPool<OrderEventFilled<MemoryPool>> pool_order_filled_;
    MemoryPool<OrderEventCancelled<MemoryPool>> pool_order_cancelled_;
    MemoryPool<OrderEventTriggered<MemoryPool>> pool_order_triggered_;
    MemoryPool<OrderEventDeactivated<MemoryPool>> pool_order_deactivated_;

  public:
    OrderEventParser(std::size_t pool_size,
                     aos::TradingPairFactoryInterface& trading_pair_factory)
        : trading_pair_factory_(trading_pair_factory),
          pool_order_new_(pool_size),
          pool_order_partially_filled_(pool_size),
          pool_order_untriggered_(pool_size),
          pool_order_rejected_(pool_size),
          pool_order_partially_filled_canceled_(pool_size),
          pool_order_filled_(pool_size),
          pool_order_cancelled_(pool_size),
          pool_order_triggered_(pool_size),
          pool_order_deactivated_(pool_size) {}

    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) override {
        auto data_json    = doc["data"];
        auto array_result = data_json.get_array();
        if (array_result.error() != simdjson::SUCCESS) return {false, nullptr};
        for (auto item : array_result.value()) {
            auto price = item["price"].get_double_in_string();
            if (price.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto leaves_qty = item["leavesQty"].get_double_in_string();
            if (leaves_qty.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto symbol = item["symbol"];
            if (symbol.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto order_id = item["orderLinkId"].get_uint64_in_string();
            if (order_id.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto order_status = item["orderStatus"];
            if (order_status.error() != simdjson::SUCCESS)
                return std::make_pair(false, nullptr);
            auto it = factory_map_.find(order_status);

            if (it != factory_map_.end()) {
                auto ptr = it->second();
                ptr->SetPrice(price.value());
                ptr->SetLeavesQty(leaves_qty.value());
                ptr->SetOrderId(order_id.value());
                auto [status, trading_pair] =
                    trading_pair_factory_.Produce(symbol.value());
                if (!status) return std::make_pair(false, nullptr);
                ptr->SetTradingPair(trading_pair);

                return std::make_pair(true, ptr);
            }
        }
        return {false, nullptr};
    }

  private:
    void RegisterFromConfig() {
        factory_map_["New"] = [this]() {
            auto ptr = pool_order_new_.Allocate();
            ptr->SetMemoryPool(&pool_order_new_);
            return ptr;
        };
        factory_map_["PartiallyFilled"] = [this]() {
            auto ptr = pool_order_partially_filled_.Allocate();
            ptr->SetMemoryPool(&pool_order_partially_filled_);
            return ptr;
        };
        factory_map_["Untriggered"] = [this]() {
            auto ptr = pool_order_untriggered_.Allocate();
            ptr->SetMemoryPool(&pool_order_untriggered_);
            return ptr;
        };
        factory_map_["Rejected"] = [this]() {
            auto ptr = pool_order_rejected_.Allocate();
            ptr->SetMemoryPool(&pool_order_rejected_);
            return ptr;
        };
        factory_map_["PartiallyFilledCanceled"] = [this]() {
            auto ptr = pool_order_partially_filled_canceled_.Allocate();
            ptr->SetMemoryPool(&pool_order_partially_filled_canceled_);
            return ptr;
        };
        factory_map_["Filled"] = [this]() {
            auto ptr = pool_order_filled_.Allocate();
            ptr->SetMemoryPool(&pool_order_filled_);
            return ptr;
        };
        factory_map_["Cancelled"] = [this]() {
            auto ptr = pool_order_cancelled_.Allocate();
            ptr->SetMemoryPool(&pool_order_cancelled_);
            return ptr;
        };
        factory_map_["Triggered"] = [this]() {
            auto ptr = pool_order_triggered_.Allocate();
            ptr->SetMemoryPool(&pool_order_triggered_);
            return ptr;
        };
        factory_map_["Deactivated"] = [this]() {
            auto ptr = pool_order_deactivated_.Allocate();
            ptr->SetMemoryPool(&pool_order_deactivated_);
            return ptr;
        };
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe