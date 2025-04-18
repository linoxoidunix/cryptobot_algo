#pragma once
#include "aoe/bybit/order_event/i_order_event.h"
#include "aoe/bybit/order_storage/i_order_storage.h"
#include "aoe/bybit/order_types/i_order_type.h"
#include "aos/order_manager/i_order_manager.h"
namespace aoe {
namespace bybit {
namespace impl {
template <template <typename> typename MemoryPool>
class OrderManager : public aos::OrderManagerInterface<MemoryPool> {
    OrderStorageInterface& order_storage_;

  public:
    OrderManager(OrderStorageInterface& order_storage)
        : order_storage_(order_storage) {}
    void PlaceOrder(boost::intrusive_ptr<aos::OrderTypeInterface<MemoryPool>>
                        order) override {
        auto typed = boost::static_pointer_cast<
            aoe::bybit::OrderTypeInterface<MemoryPool>>(order);
        // async логика для биржи
        Order placed_order{typed->Category(),  typed->OrderSide(),
                           typed->OrderMode(), OrderStatus::kInvalid,
                           typed->OrderId(),   typed->TradingPair(),
                           typed->Price(),     typed->Qty()};
        order_storage_.Add(std::move(placed_order));
    }

    void OnResponse(boost::intrusive_ptr<aos::OrderEventInterface<MemoryPool>>
                        event) override {
        auto typed = boost::static_pointer_cast<
            aoe::bybit::OrderEventInterface<MemoryPool>>(event);
        // async логика для ответа
    }

    // bool CancelOrder(OrderRouting routing, uint64_t order_id) override {
    //     // отмена
    //     return true;
    // }
};

};  // namespace impl
};  // namespace bybit
};  // namespace aoe