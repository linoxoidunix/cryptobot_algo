#pragma once

#include <boost/asio.hpp>
#include <boost/intrusive_ptr.hpp>
#include <deque>
#include <memory>

#include "aos/market_triplet_manager/i_market_triplet_manager.h"
#include "aos/mutual_information/i_mutual_information_calculator.h"
#include "aos/sliding_window_storage/i_sliding_window_storage.h"
#include "aot/Logger.h"

namespace aos {
namespace impl {

template <class T, class HashT>
class RealTimeMutualInformation {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    boost::asio::thread_pool& thread_pool_;

  public:
    RealTimeMutualInformation(
        boost::asio::thread_pool& thread_pool, int bins,
        boost::intrusive_ptr<aos::IMutualInformationCalculator<
            T, common::MemoryPoolNotThreadSafety>>
            mi_calculator,
        boost::intrusive_ptr<aos::IMarketTripletManager<
            HashT, common::MemoryPoolNotThreadSafety>>
            market_triplet_manager,
        boost::intrusive_ptr<aos::ISlidingWindowStorage<
            HashT, T, common::MemoryPoolNotThreadSafety>>
            sliding_window_storage)
        : thread_pool_(thread_pool),
          strand_(boost::asio::make_strand(thread_pool)),
          bins_(bins),
          mi_calculator_(mi_calculator),
          market_triplet_manager_(market_triplet_manager),
          sliding_window_storage_(sliding_window_storage) {}

    void AddDataPoint(const HashT& hash_asset, const T& value) {
        boost::asio::post(strand_, [this, hash_asset, value]() {
            sliding_window_storage_->AddData(hash_asset, value);
            ComputeIfReady(hash_asset);
        });
    }
    ~RealTimeMutualInformation() {
        logi("{}", "RealTimeMutualInformation dtor");
    }
    void Block() {
        std::promise<void> promise;
        std::future<void> future = promise.get_future();

        boost::asio::dispatch(strand_, [&promise]() {
            promise.set_value();  // Завершаем ожидание, когда все задачи на
                                  // strand выполнены
        });

        future.wait();  // Блокируем текущий поток до завершения всех задач в
                        // strand_
    }

  private:
    void ComputeIfReady(const std::size_t& hash_asset) {
        if (!market_triplet_manager_->HasPair(hash_asset)) return;
        for (const auto& paired_asset :
             market_triplet_manager_->GetPairs(hash_asset)) {
            if (sliding_window_storage_->HasEnoughData(hash_asset) &&
                sliding_window_storage_->HasEnoughData(paired_asset)) {
                auto [min_status, min_hash_asset] =
                    sliding_window_storage_->GetMin(hash_asset);
                auto [max_status, max_hash_asset] =
                    sliding_window_storage_->GetMax(hash_asset);
                auto [min_paired_status, min_paired_asset] =
                    sliding_window_storage_->GetMin(paired_asset);
                auto [max_paired_status, max_paired_asset] =
                    sliding_window_storage_->GetMax(paired_asset);

                if (!min_status) continue;
                if (!max_status) continue;
                if (!min_paired_status) continue;
                if (!max_paired_status) continue;

                sliding_window_storage_->GetMax(paired_asset);
                T mi = mi_calculator_->ComputeMutualInformation(
                    sliding_window_storage_->GetData(hash_asset),
                    min_hash_asset, max_hash_asset,
                    sliding_window_storage_->GetData(paired_asset),
                    min_paired_asset, max_paired_asset, bins_);

                logi("Mutual Information ({} ↔ {}):{}", hash_asset,
                     paired_asset, mi);
            }
        }
    }
    int bins_;
    boost::intrusive_ptr<
        aos::IMutualInformationCalculator<T, common::MemoryPoolNotThreadSafety>>
        mi_calculator_;
    boost::intrusive_ptr<
        aos::IMarketTripletManager<HashT, common::MemoryPoolNotThreadSafety>>
        market_triplet_manager_;
    boost::intrusive_ptr<
        aos::ISlidingWindowStorage<HashT, T, common::MemoryPoolNotThreadSafety>>
        sliding_window_storage_;
};
};  // namespace impl
};  // namespace aos