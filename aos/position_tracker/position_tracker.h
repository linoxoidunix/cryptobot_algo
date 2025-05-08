#pragma once
#include <list>

#include "aos/pnl/i_pnl.h"
#include "aos/position_tracker/i_position_tracker.h"
#include "aos/trading_pair/trading_pair.h"
#include "boost/intrusive_ptr.hpp"
namespace aos {
namespace impl {
template <typename Price, typename Qty, template <typename> typename MemoryPool>
class PositionTracker : public IPositionTracker<Price, Qty, MemoryPool> {
    std::list<boost::intrusive_ptr<IPositionTracker<Price, Qty, MemoryPool>>>
        observers_;

  public:
    PositionTracker() {}
    ~PositionTracker() override {}
    void AddObserver(
        boost::intrusive_ptr<IPositionTracker<Price, Qty, MemoryPool>>
            observer) {
        observers_.push_back(observer);
    }
    void AddPosition(common::ExchangeId exchange, aos::TradingPair assetPair,
                     Price price, Qty qty) override {
        for (auto& observer : observers_) {
            observer->AddPosition(exchange, assetPair, price, qty);
        }
    };
    bool RemovePosition(common::ExchangeId exchange, aos::TradingPair assetPair,
                        Price price, Qty qty) override {
        auto status = true;
        for (auto& observer : observers_) {
            auto status_local =
                observer->RemovePosition(exchange, assetPair, price, qty);
            status &= status_local;
        }
        return status;
    };
};
};  // namespace impl
};  // namespace aos