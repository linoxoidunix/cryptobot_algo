#pragma once
#include <unordered_set>

#include "aos/common/ref_counted.h"
namespace aos {

template <typename T, template <typename> class MemoryPool>
class IMarketTripletManager
    : public common::RefCounted<MemoryPool,
                                aos::IMarketTripletManager<T, MemoryPool>> {
  public:
    virtual void Connect(const T& hash_asset_x, const T& hash_asset_y)  = 0;
    virtual const std::unordered_set<T>& GetPairs(const T& asset) const = 0;
    virtual bool HasPair(const T& asset) const                          = 0;
    virtual ~IMarketTripletManager() = default;
};
};  // namespace aos