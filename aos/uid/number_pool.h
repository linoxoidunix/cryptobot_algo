#pragma once
#include <boost/intrusive_ptr.hpp>
#include <queue>

#include "aos/common/mem_pool.h"
#include "aos/logger/mylog.h"
#include "aos/uid/i_number_pool.h"

namespace aos {
namespace impl {
template <typename T>
class NumberPoolDefault : public NumberPoolInterface<T> {
  public:
    void ReturnToPool(size_t id) override { pool_.push(id); }
    ~NumberPoolDefault() override { logi("NumberPoolDefault dtor"); };
    std::pair<bool, size_t> GetFromPool() override {
        if (pool_.empty()) {
            return {false, 0};  // Возвращаем 0, если пул пуст
        }
        size_t id = pool_.front();
        pool_.pop();
        return {true, id};
    }

  private:
    std::queue<size_t> pool_;
};

template <typename T>
class NumberPool
    : public aos::INumberPool<T, common::MemoryPoolNotThreadSafety> {
  public:
    void ReturnToPool(size_t id) override { pool_.push(id); }
    ~NumberPool() override { logi("NumberPool dtor"); };
    std::pair<bool, size_t> GetFromPool() override {
        if (pool_.empty()) {
            return {false, 0};  // Возвращаем 0, если пул пуст
        }
        size_t id = pool_.front();
        pool_.pop();
        return {true, id};
    }

  private:
    std::queue<size_t> pool_;
};

template <typename T>
class NumberPoolBuilder {
  public:
    explicit NumberPoolBuilder(
        common::MemoryPoolNotThreadSafety<NumberPool<T>>& pool_)
        : pool_(pool_) {}

    boost::intrusive_ptr<INumberPool<T, common::MemoryPoolNotThreadSafety>>
    build() {
        auto* obj = pool_.Allocate();
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<
            INumberPool<T, common::MemoryPoolNotThreadSafety>>(obj);
    }

  private:
    common::MemoryPoolNotThreadSafety<NumberPool<T>>& pool_;
};
};  // namespace impl
};  // namespace aos