#pragma once
#include "aos/common/multi_pool.h"
#include "boost/intrusive_ptr.hpp"

namespace aoe {
namespace bybit {
namespace impl {
template <template <typename> typename Pool, typename PositionT,
          template <template <typename> typename, typename> typename EventT>
struct EventDescriptor {
    std::string_view category;
    std::string_view side;

    auto RegisterTo(auto& factory_map, auto& memory_pools) const {
        using ConcreteEvent           = EventT<Pool, PositionT>;
        factory_map[{category, side}] = [&memory_pools]() {
            auto& pool = memory_pools.template Get<ConcreteEvent>();
            return boost::intrusive_ptr<ConcreteEvent>(pool.Allocate());
        };
    }
};

template <template <typename> typename Pool, typename PositionT>
auto GetDefaultEventConfig() {
    using ED = EventDescriptor<Pool, PositionT, ExecutionEventSpotBuyDefault>;
    return std::vector{
        EventDescriptor<Pool, PositionT, ExecutionEventSpotBuyDefault>{"spot",
                                                                       "Buy"},
        EventDescriptor<Pool, PositionT, ExecutionEventSpotSellDefault>{"spot",
                                                                        "Sell"},
        EventDescriptor<Pool, PositionT, ExecutionEventLinearBuyDefault>{
            "linear", "Buy"},
        EventDescriptor<Pool, PositionT, ExecutionEventLinearSellDefault>{
            "linear", "Sell"}};
};

template <template <typename> typename Pool, typename PositionT,
          typename... PoolTypes>
class BybitExecutionEventParser {
    using EventPtr =
        boost::intrusive_ptr<ExecutionEventInterface<Pool, PositionT>>;
    using Key       = std::pair<std::string_view, std::string_view>;
    using FactoryFn = std::function<EventPtr()>;

    struct PairHash {
        std::size_t operator()(const Key& k) const {
            return std::hash<std::string_view>{}(k.first) ^
                   std::hash<std::string_view>{}(k.second);
        }
    };

    MemoryPoolHolder<Pool, ExecutionEventSpotBuyDefault<Pool, PositionT>,
                     ExecutionEventSpotSellDefault<Pool, PositionT>,
                     ExecutionEventLinearBuyDefault<Pool, PositionT>,
                     ExecutionEventLinearSellDefault<Pool, PositionT>,
                     PoolTypes...>
        memory_pools_;

    std::unordered_map<Key, FactoryFn, PairHash> factory_map_;

  public:
    explicit BybitExecutionEventParser(std::size_t pool_size)
        : memory_pools_(pool_size) {
        RegisterDefaults();
    }

    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) {
        auto data_json = doc["data"];
        if (data_json.is_array()) {
            for (auto item : data_json.get_array()) {
                std::string_view category = item["category"];
                std::string_view side     = item["side"];
                auto it                   = factory_map_.find({category, side});
                if (it != factory_map_.end()) {
                    return std::make_pair(true, it->second());
                }
            }
        }
        return {false, nullptr};
    }

    void RegisterFromConfig(const auto& config) {
        for (const auto& descriptor : config) {
            descriptor.RegisterTo(factory_map_, memory_pools_);
        }
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe