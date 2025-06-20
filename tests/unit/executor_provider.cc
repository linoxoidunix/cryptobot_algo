#include "aos/executer_provider/executor_provider.h"

#include "aos/common/mem_pool.h"
#include "gtest/gtest.h"
#include "memory"

// Helper function to initialize the parser manager

class ExecutorProviderTest : public ::testing::Test {
  protected:
    void SetUp() override {
        thread_pool_ = std::make_unique<boost::asio::thread_pool>(
            4);  // Используем 4 потока
        provider_ = std::make_unique<aos::impl::ExecutorProvider<
            size_t, common::MemoryPoolNotThreadSafety>>(*thread_pool_);
    }

    void TearDown() override { thread_pool_->join(); }

    std::unique_ptr<boost::asio::thread_pool> thread_pool_;
    std::unique_ptr<
        aos::impl::ExecutorProvider<size_t, common::MemoryPoolNotThreadSafety>>
        provider_;
};

TEST_F(ExecutorProviderTest, CreateStrandForNewAsset) {
    std::size_t hash_asset1 = std::hash<std::string>{}("asset1");

    // Получаем strand для первого активы
    auto& strand1           = provider_->GetStrand(hash_asset1);

    // Убедимся, что strand для asset1 был создан
    ASSERT_NE(&strand1, nullptr);
}

TEST_F(ExecutorProviderTest, CreateUniqueStrandForDifferentAssets) {
    std::size_t hash_asset1 = std::hash<std::string>{}("asset1");
    std::size_t hash_asset2 = std::hash<std::string>{}("asset2");

    // Получаем strand для двух разных активов
    auto& strand1           = provider_->GetStrand(hash_asset1);
    auto& strand2           = provider_->GetStrand(hash_asset2);

    // Убедимся, что для разных активов созданы разные strand
    ASSERT_NE(&strand1, &strand2);
}

TEST_F(ExecutorProviderTest, SameStrandForSameAsset) {
    std::size_t hash_asset = std::hash<std::string>{}("asset");

    // Получаем strand для того же активы несколько раз
    auto& strand1          = provider_->GetStrand(hash_asset);
    auto& strand2          = provider_->GetStrand(hash_asset);

    // Убедимся, что для одного и того же активы всегда возвращается один и тот
    // же strand
    ASSERT_EQ(&strand1, &strand2);
}

TEST_F(ExecutorProviderTest, MultipleThreadsAccess) {
    std::size_t hash_asset1 = std::hash<std::string>{}("asset1");
    std::size_t hash_asset2 = std::hash<std::string>{}("asset2");

    // Используем несколько потоков для доступа к одному и тому же активу
    std::atomic<bool> done{false};

    std::vector<std::thread> threads;
    threads.reserve(10);
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([this, &done, hash_asset1]() {
            provider_->GetStrand(hash_asset1);
            done.store(true, std::memory_order_release);
        });
    }

    // Ждем, пока все потоки не завершат работу
    for (auto& thread : threads) {
        thread.join();
    }

    // Убедимся, что поток, который создал strand, был единственным, кто это
    // сделал
    ASSERT_TRUE(done.load(std::memory_order_acquire));
}

TEST_F(ExecutorProviderTest, CorrectStrandForMultipleAssetsInParallel) {
    std::size_t hash_asset1 = std::hash<std::string>{}("asset1");
    std::size_t hash_asset2 = std::hash<std::string>{}("asset2");

    std::atomic<bool> done1{false};
    std::atomic<bool> done2{false};

    std::thread thread1([this, &done1, hash_asset1]() {
        provider_->GetStrand(hash_asset1);
        done1.store(true, std::memory_order_release);
    });

    std::thread thread2([this, &done2, hash_asset2]() {
        provider_->GetStrand(hash_asset2);
        done2.store(true, std::memory_order_release);
    });

    thread1.join();
    thread2.join();

    // Проверяем, что оба потока завершились, и strand для каждого активов
    // правильно создан
    ASSERT_TRUE(done1.load(std::memory_order_acquire));
    ASSERT_TRUE(done2.load(std::memory_order_acquire));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}