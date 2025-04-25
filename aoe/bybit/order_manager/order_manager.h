#pragma once
#include "aoe/bybit/bybit_api/i_exchange_api.h"
#include "aoe/bybit/order_event/i_order_event.h"
#include "aoe/bybit/order_manager/i_order_manager.h"
#include "aoe/bybit/order_storage/i_order_storage.h"
#include "aoe/bybit/request/amend_order/i_request.h"
#include "aoe/bybit/request/cancel_order/i_request.h"
#include "aoe/bybit/request/place_order/i_request.h"
#include "aos/uid/i_uid_manager.h"
namespace aoe {
namespace bybit {
namespace impl {
template <template <typename> typename MemoryPool>
class OrderManager : public OrderManagerInterface<MemoryPool> {
    OrderStorageInterface& order_storage_;
    aoe::bybit::SingleOrderAPIInterface<MemoryPool>& bybit_single_order_api_;
    aos::UIDManagerInterface<uint64_t>& uid_manager_;

  public:
    OrderManager(
        OrderStorageInterface& order_storage,
        aoe::bybit::SingleOrderAPIInterface<MemoryPool>& bybit_single_order_api,
        aos::UIDManagerInterface<uint64_t>& uid_manager)
        : order_storage_(order_storage),
          bybit_single_order_api_(bybit_single_order_api),
          uid_manager_(uid_manager) {}
    void PlaceOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                        order) override {
        auto typed = boost::static_pointer_cast<
            aoe::bybit::place_order::RequestInterface<MemoryPool>>(order);
        // save to storage
        typed->SetOrderId(uid_manager_.GetUniqueID());
        Order placed_order{
            typed->Category(),     typed->OrderSide(),    typed->OrderMode(),
            OrderStatus::kInvalid, PendingAction::kPlace, typed->OrderId(),
            typed->TradingPair(),  typed->Price(),        typed->Qty()};
        order_storage_.Add(std::move(placed_order));
        // send to exchange
        bybit_single_order_api_.PlaceOrder(order);
    }
    void CancelOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                         order) override {
        auto typed = boost::static_pointer_cast<
            aoe::bybit::cancel_order::RequestInterface<MemoryPool>>(order);
        order_storage_.UpdateStatus(typed->OrderId(),
                                    aoe::bybit::OrderStatus::kInvalid,
                                    aoe::bybit::PendingAction::kCancel);
        // send to exchange
        bybit_single_order_api_.CancelOrder(order);
    }
    void AmendOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                        order) override {
        auto typed = boost::static_pointer_cast<
            aoe::bybit::amend_order::RequestInterface<MemoryPool>>(order);
        // save to storage
        order_storage_.UpdateStatus(typed->OrderId(),
                                    aoe::bybit::OrderStatus::kInvalid,
                                    aoe::bybit::PendingAction::kAmend);
        // send to exchange
        bybit_single_order_api_.AmendOrder(order);
    }

    void OnResponse(boost::intrusive_ptr<aos::OrderEventInterface<MemoryPool>>
                        event) override {
        // need capture ptr to lambda
        auto typed = boost::static_pointer_cast<
            aoe::bybit::OrderEventInterface<MemoryPool>>(event);
        // async логика для ответа
        typed->Accept(*this);
    }
    void UpdateStatus(uint64_t order_id,
                      aoe::bybit::OrderStatus new_order_status,
                      aoe::bybit::PendingAction new_pending_action) override {
        order_storage_.UpdateStatus(order_id, new_order_status,
                                    new_pending_action);
    }

    void Remove(uint64_t order_id) override {
        order_storage_.Remove(order_id);
        uid_manager_.ReturnIDToPool(order_id);
    }

    void UpdateState(uint64_t order_id, double new_price,
                     double new_qty) override {
        order_storage_.UpdateState(order_id, new_price, new_qty);
    }
    void Add(impl::Order&& order) override {
        order_storage_.Add(std::move(order));
    }
    // bool CancelOrder(OrderRouting routing, uint64_t order_id) override {
    //     // отмена
    //     return true;
    // }
};

};  // namespace impl
};  // namespace bybit
};  // namespace aoe