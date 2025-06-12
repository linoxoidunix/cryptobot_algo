#pragma once

#include "aos/uid/i_uid_manager.h"
#include "aos/uid/number_pool.h"
#include "aos/uid/unique_id_generator.h"
#include "aos/common/mem_pool.h"
#include "boost/intrusive_ptr.hpp"
#include "fmtlog.h"

namespace aos {
namespace impl {

template <typename T>
class UIDManagerDefault : public UIDManagerInterface<T> {
  public:
    UIDManagerDefault(UniqueIDGeneratorInterface<T>& generator,
                      NumberPoolInterface<T>& pool)
        : id_generator_(generator), number_pool_(pool) {}

    T GetUniqueID() {
        auto [status, id_from_pool] = number_pool_.GetFromPool();
        if (!status) {
            return id_generator_.GenerateUniqueID();
        }
        return id_from_pool;
    }

    void ReturnIDToPool(T id) { number_pool_.ReturnToPool(id); }
    ~UIDManagerDefault() { logi("UIDManagerDefault dtor"); }

  private:
    UniqueIDGeneratorInterface<T>& id_generator_;
    NumberPoolInterface<T>& number_pool_;
};

template <typename T>
class UIDManager
    : public IUIDManager<
          T,
          common::
              MemoryPoolNotThreadSafety>  // NOLINT(modernize-use-equals-default)
{
  public:
    UIDManager(
        boost::intrusive_ptr<
            IUniqueIDGenerator<T, common::MemoryPoolNotThreadSafety>>
            generator,
        boost::intrusive_ptr<INumberPool<T, common::MemoryPoolNotThreadSafety>>
            pool)
        : id_generator_(generator), number_pool_(pool) {}

    T GetUniqueID() {
        auto [status, id_from_pool] = number_pool_->GetFromPool();
        if (!status) {
            return id_generator_->GenerateUniqueID();
        }
        return id_from_pool;
    }

    void ReturnIDToPool(T id) { number_pool_->ReturnToPool(id); }
    ~UIDManager() { logi("UIDManager dtor"); }

  private:
    boost::intrusive_ptr<
        IUniqueIDGenerator<T, common::MemoryPoolNotThreadSafety>>
        id_generator_;
    boost::intrusive_ptr<INumberPool<T, common::MemoryPoolNotThreadSafety>>
        number_pool_;
};

template <typename T>
class UIDManagerBuilder {
  public:
    UIDManagerBuilder(
        common::MemoryPoolNotThreadSafety<aos::impl::NumberPool<T>>&
            number_pool,
        common::MemoryPoolNotThreadSafety<aos::impl::UIDGenerator<T>>&
            uid_generator_pool,
        common::MemoryPoolNotThreadSafety<aos::impl::UIDManager<T>>&
            uid_manager_pool)
        : number_pool_(number_pool),
          uid_generator_pool_(uid_generator_pool),
          uid_manager_pool_(uid_manager_pool) {}

    boost::intrusive_ptr<IUIDManager<T, common::MemoryPoolNotThreadSafety>>
    build() {
        auto ptr_number = aos::impl::NumberPoolBuilder(number_pool_).build();
        auto ptr_uid_generator =
            aos::impl::UIDGeneratorBuilder(uid_generator_pool_).build();
        auto* obj = uid_manager_pool_.Allocate(ptr_uid_generator, ptr_number);
        obj->SetMemoryPool(&uid_manager_pool_);

        return boost::intrusive_ptr<
            IUIDManager<T, common::MemoryPoolNotThreadSafety>>(obj);
    }

  private:
    common::MemoryPoolNotThreadSafety<aos::impl::NumberPool<T>>& number_pool_;
    common::MemoryPoolNotThreadSafety<aos::impl::UIDGenerator<T>>&
        uid_generator_pool_;
    // NumberPoolBuilder<T> number_pool_builder_;
    // UIDGeneratorBuilder<T> uid_generator_builder_;
    common::MemoryPoolNotThreadSafety<aos::impl::UIDManager<T>>&
        uid_manager_pool_;
};

class UIDManagerContainer {
    common::MemoryPoolNotThreadSafety<aos::impl::NumberPool<size_t>>
        number_pool_;
    common::MemoryPoolNotThreadSafety<aos::impl::UIDGenerator<size_t>>
        uid_generator_pool_;
    common::MemoryPoolNotThreadSafety<aos::impl::UIDManager<size_t>>
        uid_manager_pool_;

  public:
    UIDManagerContainer(size_t size)
        : number_pool_(size),
          uid_generator_pool_(size),
          uid_manager_pool_(size) {};
    boost::intrusive_ptr<IUIDManager<size_t, common::MemoryPoolNotThreadSafety>>
    Build() {
        UIDManagerBuilder<size_t> builder(number_pool_, uid_generator_pool_,
                                          uid_manager_pool_);
        return builder.build();
    }
};
};  // namespace impl
};  // namespace aos