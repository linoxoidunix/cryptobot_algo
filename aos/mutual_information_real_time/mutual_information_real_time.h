#pragma once

#include <boost/asio.hpp>
#include <boost/intrusive_ptr.hpp>
#include <deque>
#include <memory>

#include "aos/market_triplet_manager/i_market_triplet_manager.h"
#include "aos/mutual_information/i_mutual_information_calculator.h"
#include "aos/sliding_window_storage/i_sliding_window_storage.h"
#include "fmtlog.h"

namespace aos {
namespace impl {

template <class T, class HashT, template <typename> typename MemoryPoolNotThreadSafety>
class RealTimeMutualInformation {
    boost::asio::strand<boost::asio::thread_pool::executor_type> strand_;
    boost::asio::thread_pool& thread_pool_;

  public:
    RealTimeMutualInformation(
        boost::asio::thread_pool& thread_pool, int bins,
        boost::intrusive_ptr<aos::IMutualInformationCalculator<
            T, MemoryPoolNotThreadSafety, HashT,
            MemoryPoolNotThreadSafety>>
            mi_calculator,
        boost::intrusive_ptr<aos::IMarketTripletManager<
            HashT, MemoryPoolNotThreadSafety>>
            market_triplet_manager,
        boost::intrusive_ptr<aos::ISlidingWindowStorage<
            HashT, T, MemoryPoolNotThreadSafety>>
            sliding_window_storage)
        : thread_pool_(thread_pool),
          strand_(boost::asio::make_strand(thread_pool)),
          bins_(bins),
          mi_calculator_(mi_calculator),
          market_triplet_manager_(market_triplet_manager),
          sliding_window_storage_(sliding_window_storage) {}

    void AddDataPoint(const HashT& hash_asset, const T& value) {
        boost::asio::post(strand_, [this, hash_asset, value]() {
            auto [status, result] =
                sliding_window_storage_->IsDeviationAboveThreshold(hash_asset,
                                                                   value, 0.5);
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
    void ComputeIfReady(const HashT& hash_asset) {
        if (!market_triplet_manager_->HasPair(hash_asset)) return;
        for (const auto& paired_asset :
             market_triplet_manager_->GetPairs(hash_asset)) {
            auto [status, mi] = mi_calculator_->ComputeMutualInformation(
                sliding_window_storage_, hash_asset, paired_asset, bins_);
            if (!status) continue;
            logi("Mutual Information ({} ↔ {}):{}", hash_asset, paired_asset,
                 mi);
        }
    }
    int bins_;
    boost::intrusive_ptr<aos::IMutualInformationCalculator<
        T, MemoryPoolNotThreadSafety, HashT,
        MemoryPoolNotThreadSafety>>
        mi_calculator_;
    boost::intrusive_ptr<
        aos::IMarketTripletManager<HashT, MemoryPoolNotThreadSafety>>
        market_triplet_manager_;
    boost::intrusive_ptr<
        aos::ISlidingWindowStorage<HashT, T, MemoryPoolNotThreadSafety>>
        sliding_window_storage_;
};
};  // namespace impl
};  // namespace aos