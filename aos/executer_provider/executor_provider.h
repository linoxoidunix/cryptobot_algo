#pragma once
#include <atomic>

#include "aos/executer_provider/i_executor_provider.h"
#include "unordered_map"

namespace aos {
namespace impl {

template <class HashT, template <typename> typename MemoryPoolNotThreadSafety>
class ExecutorProvider
    : public IExecutorProvider<HashT, MemoryPoolNotThreadSafety> {
  public:
    explicit ExecutorProvider(boost::asio::thread_pool& thread_pool)
        : thread_pool_(thread_pool) {}

    boost::asio::strand<boost::asio::thread_pool::executor_type>& GetStrand(
        const HashT hash_asset) override {
        auto& flag = flags_[hash_asset];
        // auto& strand = strands_[hash_asset];

        // Проверка или создание strand
        if (!flag.load(std::memory_order_acquire)) {
            bool expected = false;
            if (flag.compare_exchange_strong(expected, true,
                                             std::memory_order_release)) {
                // Если флаг был false, создаем strand
                auto strand =
                    boost::asio::make_strand(thread_pool_.get_executor());
                strands_.insert_or_assign(hash_asset, strand);
            } else {
                // Если флаг был уже установлен, ждем его активации
                while (!flag.load(std::memory_order_acquire)) {
                    std::this_thread::yield();  // Позволяет другим потокам
                                                // работать
                }
            }
        }

        return strands_.at(hash_asset);
    }

  private:
    boost::asio::thread_pool& thread_pool_;
    std::unordered_map<
        std::size_t,
        boost::asio::strand<boost::asio::thread_pool::executor_type>>
        strands_;
    std::unordered_map<std::size_t, std::atomic<bool>> flags_;
};

template <typename HashT, template <typename> typename MemoryPoolNotThreadSafety>
class ExecutorProviderBuilder {
  public:
    explicit ExecutorProviderBuilder(
        MemoryPoolNotThreadSafety<ExecutorProvider<HashT, MemoryPoolNotThreadSafety>>& pool,
        boost::asio::thread_pool& thread_pool)
        : pool_(pool), thread_pool_(thread_pool) {}

    boost::intrusive_ptr<ExecutorProvider<HashT, MemoryPoolNotThreadSafety>> build() {
        auto* obj = pool_.Allocate(thread_pool_);
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<ExecutorProvider<HashT, MemoryPoolNotThreadSafety>>(obj);
    }

  private:
    MemoryPoolNotThreadSafety<ExecutorProvider<HashT, MemoryPoolNotThreadSafety>>& pool_;
    boost::asio::thread_pool& thread_pool_;
};
}  // namespace impl
}  // namespace aos