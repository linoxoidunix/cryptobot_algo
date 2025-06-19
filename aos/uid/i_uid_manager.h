#pragma once

#include "aos/common/ref_counted.h"

namespace aos {

template <typename T>
class UIDManagerInterface {
  public:
    virtual ~UIDManagerInterface()    = default;

    virtual T GetUniqueID()           = 0;
    virtual void ReturnIDToPool(T id) = 0;
};
template <typename T, template <typename> class MemoryPool>
class IUIDManager
    : public common::RefCounted<MemoryPool, aos::IUIDManager<T, MemoryPool>> {
  public:
    ~IUIDManager() override           = default;

    virtual T GetUniqueID()           = 0;
    virtual void ReturnIDToPool(T id) = 0;
};

};  // namespace aos
