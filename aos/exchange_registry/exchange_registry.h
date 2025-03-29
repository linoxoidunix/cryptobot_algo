#pragma once
#include <unordered_map>

#include "aos/exchange_registry/i_exchange_registry.h"

namespace aos {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class ExchangeRegistry
    : public aos::IExchangeRegistry<Price, Qty, MemoryPool, Uid> {
  private:
    using ExchangeKey = std::pair<OrderRouting, common::ExchangeId>;
    struct HashPair {
        size_t operator()(const ExchangeKey& key) const {
            return std::hash<int>()(static_cast<int>(key.first)) ^
                   std::hash<int>()(static_cast<int>(key.second));
        }
    };

    std::unordered_map<
        std::pair<OrderRouting, common::ExchangeId>,
        boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>>, HashPair>
        exchanges_;

  public:
    ~ExchangeRegistry() override {
        logi("ExchangeRegistry dtor");
        // for (auto it : exchanges_) {
        //     it.second = nullptr;
        // }
        // exchanges_.clear();
    }
    void RegisterExchange(
        OrderRouting routing, common::ExchangeId exchange_id,
        boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>> exchange)
        override {
        exchanges_.insert_or_assign({routing, exchange_id}, exchange);
    }

    std::pair<bool,
              boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>>>
    GetExchange(OrderRouting routing,
                common::ExchangeId exchange_id) const override {
        auto it = exchanges_.find({routing, exchange_id});
        if (it == exchanges_.end()) {
            return std::make_pair(false, nullptr);
        }
        return std::make_pair(true, it->second);
    }
};

template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename Uid>
class FakeExchangeRegistry
    : public aos::IExchangeRegistry<Price, Qty, MemoryPool, Uid> {
  private:
    using ExchangeKey = std::pair<OrderRouting, common::ExchangeId>;
    struct HashPair {
        size_t operator()(const ExchangeKey& key) const {
            return std::hash<int>()(static_cast<int>(key.first)) ^
                   std::hash<int>()(static_cast<int>(key.second));
        }
    };

    boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>> exchange_;

  public:
    ~FakeExchangeRegistry() override { logi("FakeExchangeRegistry dtor"); }
    void RegisterExchange(
        OrderRouting routing, common::ExchangeId exchange_id,
        boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>> exchange)
        override {
        exchange_ = exchange;
    }

    std::pair<bool,
              boost::intrusive_ptr<IExchange<Price, Qty, MemoryPool, Uid>>>
    GetExchange(OrderRouting routing,
                common::ExchangeId exchange_id) const override {
        return std::make_pair(false, nullptr);
    }
};

// Концепт для проверки, что RegistryT наследует IExchangeRegistry
template <typename RegistryT, typename Price, typename Qty,
          template <typename> typename MemoryPool, typename Uid>
concept IsExchangeRegistry =
    std::is_base_of_v<aos::IExchangeRegistry<Price, Qty, MemoryPool, Uid>,
                      RegistryT>;

template <typename RegistryT, typename Price, typename Qty,
          template <typename> typename MemoryPool, typename Uid>
    requires IsExchangeRegistry<RegistryT, Price, Qty, MemoryPool, Uid>
class ExchangeRegistryBuilder {
  private:
    using IRegistryType = IExchangeRegistry<Price, Qty, MemoryPool, Uid>;

    MemoryPool<ExchangeRegistry<Price, Qty, MemoryPool, Uid>>& pool_;

  public:
    ExchangeRegistryBuilder(MemoryPool<RegistryT>& pool) : pool_(pool) {}

    boost::intrusive_ptr<IRegistryType> Build() {
        auto* obj = pool_.Allocate();
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<IRegistryType>(obj);
    };
};
};  // namespace impl
};  // namespace aos