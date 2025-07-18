#pragma once
#include "aoe/bybit/order_manager/order_manager.h"
#include "aoe/bybit/order_storage/order_storage.h"
#include "aos/uid/i_uid_manager.h"
#include "aos/uid/number_pool.h"
#include "aos/uid/uid_manager.h"
#include "aos/uid/unique_id_generator.h"

namespace aoe {
namespace bybit {
namespace impl {
template <template <typename> typename MemoryPool>
class OrderManagerDefault : public OrderManagerInterface<MemoryPool> {
    aoe::bybit::impl::OrderStorage order_storage_;
    aos::impl::NumberPoolDefault<uint64_t> number_pool_;
    aos::impl::UIDGeneratorDefault<uint64_t> uid_generator_;
    aos::impl::UIDManagerDefault<uint64_t> uid_manager_;  //(uid_generator,
                                                          // number_pool);
    OrderManager<MemoryPool> order_manager_;

  public:
    explicit OrderManagerDefault(
        aoe::bybit::SingleOrderAPIInterface<MemoryPool>& bybit_single_order_api)
        : uid_manager_(uid_generator_, number_pool_),
          order_manager_(order_storage_, bybit_single_order_api, uid_manager_) {
    }
    ~OrderManagerDefault() override = default;
    void PlaceOrderManualId(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>> order,
        uint64_t uid) override {
        order_manager_.PlaceOrderManualId(order, uid);
    }
    void PlaceOrderAutoId(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>> order)
        override {
        order_manager_.PlaceOrderAutoId(order);
    }
    void CancelOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                         order) override {
        order_manager_.CancelOrder(order);
    }
    void AmendOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                        order) override {
        // send to exchange
        order_manager_.AmendOrder(order);
    }

    void OnResponse(boost::intrusive_ptr<aos::OrderEventInterface<MemoryPool>>
                        event) override {
        // need capture ptr to lambda
        order_manager_.OnResponse(event);
    }
    void UpdateStatus(uint64_t order_id,
                      aoe::bybit::OrderStatus new_order_status,
                      aoe::bybit::PendingAction new_pending_action) override {
        order_manager_.UpdateStatus(order_id, new_order_status,
                                    new_pending_action);
    }

    void Remove(uint64_t order_id) override { order_manager_.Remove(order_id); }

    void UpdateState(uint64_t order_id, double new_price,
                     double new_qty) override {
        order_manager_.UpdateState(order_id, new_price, new_qty);
    }
    void Add(impl::Order&& order) override {
        order_manager_.Add(std::move(order));
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe