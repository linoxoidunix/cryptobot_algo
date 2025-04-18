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
class SpotBuyLimitOrder : public OrderTypeInterface<MemoryPool> {
  public:
    SpotBuyLimitOrder() {
        this->SetSide(aos::Side::kBuy);
        this->SetCategory(aoe::bybit::Category::kSpot);
        this->SetOrderMode(aoe::bybit::OrderMode::kLimit);
    }
    ~SpotBuyLimitOrder() override = default;
};
template <template <typename> typename MemoryPool>
class SpotSellLimitOrder : public OrderTypeInterface<MemoryPool> {
  public:
    SpotSellLimitOrder() {
        this->SetSide(aos::Side::kSell);
        this->SetCategory(aoe::bybit::Category::kSpot);
        this->SetOrderMode(aoe::bybit::OrderMode::kLimit);
    }
    ~SpotSellLimitOrder() override = default;
};

template <template <typename> typename MemoryPool>
class SpotBuyMarketOrder : public OrderTypeInterface<MemoryPool> {
  public:
    SpotBuyMarketOrder() {
        this->SetSide(aos::Side::kBuy);
        this->SetCategory(aoe::bybit::Category::kSpot);
        this->SetOrderMode(aoe::bybit::OrderMode::kMarket);
    }
    ~SpotBuyMarketOrder() override = default;
};
template <template <typename> typename MemoryPool>
class SpotSellMarketOrder : public OrderTypeInterface<MemoryPool> {
  public:
    SpotSellMarketOrder() {
        this->SetSide(aos::Side::kSell);
        this->SetCategory(aoe::bybit::Category::kSpot);
        this->SetOrderMode(aoe::bybit::OrderMode::kMarket);
    }
    ~SpotSellMarketOrder() override = default;
};

template <template <typename> typename MemoryPool>
class LinearBuyLimitOrder : public OrderTypeInterface<MemoryPool> {
  public:
    LinearBuyLimitOrder() {
        this->SetSide(aos::Side::kBuy);
        this->SetCategory(aoe::bybit::Category::kLinear);
        this->SetOrderMode(aoe::bybit::OrderMode::kLimit);
    }
    ~LinearBuyLimitOrder() override = default;
};
template <template <typename> typename MemoryPool>
class LinearSellLimitOrder : public OrderTypeInterface<MemoryPool> {
  public:
    LinearSellLimitOrder() {
        this->SetSide(aos::Side::kSell);
        this->SetCategory(aoe::bybit::Category::kLinear);
        this->SetOrderMode(aoe::bybit::OrderMode::kLimit);
    }
    ~LinearSellLimitOrder() override = default;
};

template <template <typename> typename MemoryPool>
class LinearBuyMarketOrder : public OrderTypeInterface<MemoryPool> {
  public:
    LinearBuyMarketOrder() {
        this->SetSide(aos::Side::kBuy);
        this->SetCategory(aoe::bybit::Category::kLinear);
        this->SetOrderMode(aoe::bybit::OrderMode::kMarket);
    }
    ~LinearBuyMarketOrder() override = default;
};
template <template <typename> typename MemoryPool>
class LinearSellMarketOrder : public OrderTypeInterface<MemoryPool> {
  public:
    LinearSellMarketOrder() {
        this->SetSide(aos::Side::kSell);
        this->SetCategory(aoe::bybit::Category::kLinear);
        this->SetOrderMode(aoe::bybit::OrderMode::kMarket);
    }
    ~LinearSellMarketOrder() override = default;
};
};  // namespace impl
};  // namespace aos