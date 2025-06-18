// Copyright 2025 Denis Evlanov

#pragma once
#include "aoe/bybit/enums/enums.h"
#include "aos/common/ref_counted.h"
// #include
// "aos/position_storage/position_storage_by_pair/i_position_storage_by_pair.h"
#include "aos/common/exchange_id.h"
namespace aos {
template <template <typename> typename MemoryPool>
class OrderEventInterface
    : public common::RefCounted<MemoryPool, OrderEventInterface<MemoryPool>> {
  public:
    virtual ~OrderEventInterface() = default;
};
};  // namespace aos