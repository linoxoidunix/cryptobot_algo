#pragma once
#include "aos/common/mem_pool.h"
#include "aos/logger/mylog.h"
#include "aos/uid/i_unique_id_generator.h"

namespace aos {
namespace impl {

template <typename T>
class UIDGeneratorDefault : public UniqueIDGeneratorInterface<T> {
  public:
    UIDGeneratorDefault() = default;
    ~UIDGeneratorDefault() override { logi("UIDGenerator dtor"); }
    T GenerateUniqueID() override { return current_id_++; }

  private:
    T current_id_ = 0;
};

template <typename T>
class UIDGenerator
    : public IUniqueIDGenerator<T, common::MemoryPoolNotThreadSafety> {
  public:
    UIDGenerator() = default;
    ~UIDGenerator() override { logi("UIDGenerator dtor"); }
    T GenerateUniqueID() override { return current_id_++; }

  private:
    T current_id_ = 0;
};

template <typename T>
class UIDGeneratorBuilder {
  public:
    explicit UIDGeneratorBuilder(
        common::MemoryPoolNotThreadSafety<UIDGenerator<T>>& pool)
        : pool_(pool) {}

    boost::intrusive_ptr<
        IUniqueIDGenerator<T, common::MemoryPoolNotThreadSafety>>
    build() {
        auto* obj = pool_.Allocate();
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<
            IUniqueIDGenerator<T, common::MemoryPoolNotThreadSafety>>(obj);
    }

  private:
    common::MemoryPoolNotThreadSafety<UIDGenerator<T>>& pool_;
};
};  // namespace impl
};  // namespace aos