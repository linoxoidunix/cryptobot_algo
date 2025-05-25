#pragma once
#include "aoe/binance/leaves_qty_calculator/leaves_qty_calculator.h"
#include "aos/order_event/i_order_event.h"
namespace aoe {
namespace binance {
template <template <typename> typename MemoryPool>
class OrderEventDefault : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventDefault() = default;
    ~OrderEventDefault() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {};
};

template <template <typename> typename MemoryPool>
class OrderEventNew : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventNew() = default;
    ~OrderEventNew() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.UpdateStatus(this->order_id_,
                                   aoe::binance::OrderStatus::kNew,
                                   aoe::binance::PendingAction::kNone);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventPendingNew : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventPendingNew() = default;
    ~OrderEventPendingNew() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.UpdateStatus(this->order_id_,
                                   aoe::binance::OrderStatus::kPendingNew,
                                   aoe::binance::PendingAction::kNone);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventPartiallyFilled : public OrderEventInterface<MemoryPool> {
    impl::LeavesQtyCalculator<MemoryPool> calculator_;

  public:
    OrderEventPartiallyFilled() = default;
    ~OrderEventPartiallyFilled() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.UpdateStatus(this->order_id_,
                                   aoe::binance::OrderStatus::kPartiallyFilled,
                                   aoe::binance::PendingAction::kNone);
        // need adapter to calculate LeavesQty
        auto leaves_qty = calculator_.Calculate(*this);
        order_mutator.UpdateState(this->OrderId(), this->Price(), leaves_qty);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventFilled : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventFilled() = default;
    ~OrderEventFilled() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventCancelled : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventCancelled() = default;
    ~OrderEventCancelled() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventPendingCancel : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventPendingCancel() = default;
    ~OrderEventPendingCancel() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        // https://github.com/binance/binance-spot-api-docs/blob/master/enums.md
        // currently unused
    };
};

template <template <typename> typename MemoryPool>
class OrderEventRejected : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventRejected() = default;
    ~OrderEventRejected() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventExpired : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventExpired() = default;
    ~OrderEventExpired() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventExpiredInMatch : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventExpiredInMatch() = default;
    ~OrderEventExpiredInMatch() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};
};  // namespace binance
};  // namespace aoe