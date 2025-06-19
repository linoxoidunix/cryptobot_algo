#pragma once
#include "aos/order_event/i_order_event.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class OrderEventDefault : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventDefault()           = default;
    ~OrderEventDefault() override = default;
    void Accept(OrderMutatorInterface& order_mutator) override {};
};

template <template <typename> typename MemoryPool>
class OrderEventNew : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventNew()           = default;
    ~OrderEventNew() override = default;
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.UpdateStatus(this->order_id_,
                                   aoe::bybit::OrderStatus::kNew,
                                   aoe::bybit::PendingAction::kNone);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventPartiallyFilled : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventPartiallyFilled()           = default;
    ~OrderEventPartiallyFilled() override = default;
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.UpdateStatus(this->order_id_,
                                   aoe::bybit::OrderStatus::kPartiallyFilled,
                                   aoe::bybit::PendingAction::kNone);
        order_mutator.UpdateState(this->order_id_, this->price_,
                                  this->LeavesQty());
    };
};

template <template <typename> typename MemoryPool>
class OrderEventUntriggered : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventUntriggered()           = default;
    ~OrderEventUntriggered() override = default;
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.UpdateStatus(this->order_id_,
                                   aoe::bybit::OrderStatus::kUntriggered,
                                   aoe::bybit::PendingAction::kNone);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventRejected : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventRejected()           = default;
    ~OrderEventRejected() override = default;
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventPartiallyFilledCancelled
    : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventPartiallyFilledCancelled()           = default;
    ~OrderEventPartiallyFilledCancelled() override = default;
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventFilled : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventFilled()           = default;
    ~OrderEventFilled() override = default;
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventCancelled : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventCancelled()           = default;
    ~OrderEventCancelled() override = default;
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventTriggered : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventTriggered()           = default;
    ~OrderEventTriggered() override = default;
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventDeactivated : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventDeactivated()           = default;
    ~OrderEventDeactivated() override = default;
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};
};  // namespace bybit
};  // namespace aoe