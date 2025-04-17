#pragma once
#include "aoe/bybit/order_types/i_order_type.h"

namespace aos {
namespace impl {

template <template <typename> typename MemoryPool>
class OrderDefault : public OrderTypeInterface<MemoryPool> {
  public:
    ~OrderDefault() override = default;
};

template <template <typename> typename MemoryPool>
class SpotBuyOrderDefault : public OrderTypeInterface<MemoryPool> {
  public:
    SpotBuyOrderDefault() {
        this->SetSide(aos::Side::kBuy);
        this->SetCategory(aoe::bybit::Category::kSpot);
    }
    ~SpotBuyOrderDefault() override = default;
};
template <template <typename> typename MemoryPool>
class SpotSellOrderDefault : public OrderTypeInterface<MemoryPool> {
  public:
    SpotSellOrderDefault() {
        this->SetSide(aos::Side::kSell);
        this->SetCategory(aoe::bybit::Category::kSpot);
    }
    ~SpotSellOrderDefault() override = default;
};
template <template <typename> typename MemoryPool>
class LinearBuyOrderDefault : public OrderTypeInterface<MemoryPool> {
  public:
    LinearBuyOrderDefault() {
        this->SetSide(aos::Side::kBuy);
        this->SetCategory(aoe::bybit::Category::kLinear);
    }
    ~LinearBuyOrderDefault() override = default;
};
template <template <typename> typename MemoryPool>
class LinearSellOrderDefault : public OrderTypeInterface<MemoryPool> {
  public:
    LinearSellOrderDefault() {
        this->SetSide(aos::Side::kSell);
        this->SetCategory(aoe::bybit::Category::kLinear);
    }
    ~LinearSellOrderDefault() override = default;
};
};  // namespace impl
};  // namespace aos