#pragma once

#include "aos/common/exchange_id.h"
#include "aos/event/funding_rate/i_funding_rate_event.h"
#include "aos/trading_pair/trading_pair.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool>
class FundingRateEventInterface
    : public aos::FundingRateEventInterface<MemoryPool> {
  public:
    ~FundingRateEventInterface() override = default;
    virtual uint64_t NextFundingTime() const { return next_funding_time_; };

    virtual common::ExchangeId ExchangeId() const { return kExchangeId_; };
    virtual aos::TradingPair TradingPair() const { return trading_pair_; };

    virtual void SetTradingPair(aos::TradingPair trading_pair) {
        trading_pair_ = trading_pair;
    };
    virtual void SetNextFundingTime(uint64_t next_funding_time) {
        next_funding_time_ = next_funding_time;
    };
    virtual void SetFundingRate(double funding_rate) {
        funding_rate_ = funding_rate;
    };
    virtual double FundingRate() { return funding_rate_; };

  protected:
    uint64_t next_funding_time_     = 0;
    double funding_rate_            = 0;
    common::ExchangeId kExchangeId_ = common::ExchangeId::kBybit;
    aos::TradingPair trading_pair_;
};
};  // namespace bybit
};  // namespace aoe