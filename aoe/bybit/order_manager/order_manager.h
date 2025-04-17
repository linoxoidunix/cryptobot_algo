#pragma once
#include "aoe/bybit/order_event/i_order_event.h"
#include "aoe/bybit/order_types/i_order_type.h"
#include "aos/order_manager/i_order_manager.h"
namespace aoe {
namespace bybit {
namespace impl {
template <template <typename> typename MemoryPool>
class OrderManagerDefault : public aos::OrderManagerInterface<MemoryPool> {
  public:
    void PlaceOrder(boost::intrusive_ptr<aos::OrderTypeInterface<MemoryPool>>
                        order) override {
        auto typed = boost::static_pointer_cast<
            aoe::bybit::OrderTypeInterface<MemoryPool>>(order);
        // async логика для биржи
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