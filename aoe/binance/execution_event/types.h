#pragma once
#include "aoe/binance/execution_event/i_types.h"
namespace aoe {
namespace binance {
template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventDefault
    : public ExecutionEventInterface<MemoryPool, PositionT> {
  public:
    ExecutionEventDefault() = default;
    ~ExecutionEventDefault() override {};
    double ExecQty() const override { return 0; }
    void Accept(aos::PositionStorageByPairInterface<double, double, PositionT>&
                    position_storage) override {};
};

template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventSpotSellDefault
    : public ExecutionEventInterface<MemoryPool, PositionT> {
  public:
    ExecutionEventSpotSellDefault() = default;
    ~ExecutionEventSpotSellDefault() override {};
    double ExecQty() const override { return this->exec_qty_; }
    void Accept(aos::PositionStorageByPairInterface<double, double, PositionT>&
                    position_storage) override {
        position_storage.RemovePosition(this->ExchangeId(), this->TradingPair(),
                                        this->ExecPrice(), this->ExecQty());
    }
};

template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventSpotBuyDefault
    : public ExecutionEventInterface<MemoryPool, PositionT> {
  public:
    ExecutionEventSpotBuyDefault() = default;
    ~ExecutionEventSpotBuyDefault() override {};
    double ExecQty() const override { return this->exec_qty_; }
    void Accept(aos::PositionStorageByPairInterface<double, double, PositionT>&
                    position_storage) override {
        position_storage.AddPosition(this->ExchangeId(), this->TradingPair(),
                                     this->ExecPrice(), this->ExecQty());
    }
};

template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventFuturesSellDefault
    : public ExecutionEventInterface<MemoryPool, PositionT> {
  public:
    ExecutionEventFuturesSellDefault() = default;
    ~ExecutionEventFuturesSellDefault() override {};
    double ExecQty() const override {
        return -this->exec_qty_;  // minus is determine that sell for futures
    };
    void Accept(aos::PositionStorageByPairInterface<double, double, PositionT>&
                    position_storage) override {
        position_storage.AddPosition(this->ExchangeId(), this->TradingPair(),
                                     this->ExecPrice(), this->ExecQty());
    }
};

template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventFuturesBuyDefault
    : public ExecutionEventInterface<MemoryPool, PositionT> {
  public:
    ExecutionEventFuturesBuyDefault() = default;
    ~ExecutionEventFuturesBuyDefault() override {};
    double ExecQty() const override {
        return -this->exec_qty_;  // minus is determine that buy for futures
    };
    void Accept(aos::PositionStorageByPairInterface<double, double, PositionT>&
                    position_storage) override {
        position_storage.RemovePosition(this->ExchangeId(), this->TradingPair(),
                                        this->ExecPrice(), this->ExecQty());
    }
};
};  // namespace binance
};  // namespace aoe