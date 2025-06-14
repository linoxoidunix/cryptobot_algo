#pragma once
#include <boost/intrusive_ptr.hpp>
#include <queue>

#include "aos/uid/i_number_pool.h"
#include "aos/common/mem_pool.h"
#include "fmtlog.h"

namespace aos {
namespace impl {
template <typename T>
class NumberPoolDefault : public NumberPoolInterface<T> {
  public:
    void ReturnToPool(size_t id) override { pool.push(id); }
    ~NumberPoolDefault() override { logi("NumberPoolDefault dtor"); };
    std::pair<bool, size_t> GetFromPool() override {
        if (pool.empty()) {
            return {false, 0};  // Возвращаем 0, если пул пуст
        }
        size_t id = pool.front();
        pool.pop();
        return {true, id};
    }

  private:
    std::queue<size_t> pool;
};

template <typename T>
class NumberPool
    : public aos::INumberPool<T, common::MemoryPoolNotThreadSafety> {
  public:
    void ReturnToPool(size_t id) override { pool.push(id); }
    ~NumberPool() override { logi("NumberPool dtor"); };
    std::pair<bool, size_t> GetFromPool() override {
        if (pool.empty()) {
            return {false, 0};  // Возвращаем 0, если пул пуст
        }
        size_t id = pool.front();
        pool.pop();
        return {true, id};
    }

  private:
    std::queue<size_t> pool;
};

template <typename T>
class NumberPoolBuilder {
  public:
    explicit NumberPoolBuilder(
        common::MemoryPoolNotThreadSafety<NumberPool<T>>& pool)
        : pool_(pool) {}

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