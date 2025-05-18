#pragma once
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
class OrderEventPartiallyFilled : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventPartiallyFilled() = default;
    ~OrderEventPartiallyFilled() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.UpdateStatus(this->order_id_,
                                   aoe::binance::OrderStatus::kNew,
                                   aoe::binance::PendingAction::kNone);
        order_mutator.UpdateState(this->order_id_, this->price_,
                                  this->LeavesQty());
    };
};

template <template <typename> typename MemoryPool>
class OrderEventUntriggered : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventUntriggered() = default;
    ~OrderEventUntriggered() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {};
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
class OrderEventPartiallyFilledCancelled
    : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventPartiallyFilledCancelled() = default;
    ~OrderEventPartiallyFilledCancelled() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
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
class OrderEventTriggered : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventTriggered() = default;
    ~OrderEventTriggered() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};

template <template <typename> typename MemoryPool>
class OrderEventDeactivated : public OrderEventInterface<MemoryPool> {
  public:
    OrderEventDeactivated() = default;
    ~OrderEventDeactivated() override {};
    void Accept(OrderMutatorInterface& order_mutator) override {
        order_mutator.Remove(this->order_id_);
    };
};
};  // namespace binance
};  // namespace aoe