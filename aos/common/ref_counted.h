#pragma once
#define BOOST_INTRUSIVE_PTR_DEBUG
#include <atomic>
#include <boost/intrusive_ptr.hpp>
#include <iostream>

#include "fmtlog.h"

namespace common {
template <template <typename> class MemoryPool, typename T>
class RefCounted {
  public:
    RefCounted() : memory_pool_(nullptr) {}
    virtual ~RefCounted() = default;
    void SetMemoryPool(void *pool) { memory_pool_ = pool; }

    friend void intrusive_ptr_add_ref(RefCounted *p) {
        p->ref_count_.fetch_add(1, std::memory_order_relaxed);
    }

    friend void intrusive_ptr_release(RefCounted *p) {
        if (p->ref_count_.fetch_sub(1, std::memory_order_acquire) == 1) {
            std::atomic_thread_fence(std::memory_order_release);
            if (p->memory_pool_) {  // Проверка на nullptr
                auto *pool = static_cast<MemoryPool<T> *>(p->memory_pool_);
                pool->Deallocate(static_cast<T *>(p));
            } else {
                logi("no found memory pool to dealocate");
                // delete p;  // Без пула просто удаляем объект
            }
        }
    }

  private:
    std::atomic<int> ref_count_{0};
    void *memory_pool_;
};
};  // namespace common
