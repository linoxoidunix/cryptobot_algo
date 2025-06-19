#pragma once
#include <memory>
#include <unordered_map>

#include "aos/multi_order_manager/i_multi_order_manager.h"
#include "aos/order_manager/i_order_manager.h"
namespace aos {
namespace impl {

template <template <typename> typename MemoryPool>
class MultiOrderManagerDefault : public MultiOrderManagerInterface<MemoryPool> {
    std::unordered_map<common::ExchangeId,
                       std::unique_ptr<OrderManagerInterface<MemoryPool>>>
        oms_;

  public:
    void Register(
        common::ExchangeId id,
        std::unique_ptr<OrderManagerInterface<MemoryPool>> om) override {
        oms_[id] = std::move(om);
    }

    void PlaceOrderManualId(
        common::ExchangeId id,
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order,
        uint64_t uid) override {
        oms_.at(id)->PlaceOrderManualId(order, uid);
    }

    void PlaceOrderAutoId(
        common::ExchangeId id,
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order) override {
        oms_.at(id)->PlaceOrderAutoId(order);
    }

    void CancelOrder(
        common::ExchangeId id,
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order) override {
        oms_.at(id)->CancelOrder(order);
    }

    void AmendOrder(
        common::ExchangeId id,
        boost::intrusive_ptr<RequestInterface<MemoryPool>> order) override {
        oms_.at(id)->AmendOrder(order);
    }

    void OnResponse(
        common::ExchangeId id,
        boost::intrusive_ptr<OrderEventInterface<MemoryPool>> event) override {
        oms_.at(id)->OnResponse(event);
    }
    ~MultiOrderManagerDefault() override = default;
};

};  // namespace impl
};  // namespace aos