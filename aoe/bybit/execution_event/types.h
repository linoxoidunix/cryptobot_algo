#pragma once
#include "aoe/bybit/execution_event/i_types.h"
namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventDefault
    : public ExecutionEventInterface<MemoryPool, PositionT> {
  public:
    ExecutionEventDefault()           = default;
    ~ExecutionEventDefault() override = default;
    double ExecQty() const override { return 0; }
    void Accept(aos::PositionStorageByPairInterface<double, double, PositionT>&
                    position_storage) override {};
};

template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventSpotSellDefault
    : public ExecutionEventInterface<MemoryPool, PositionT> {
  public:
    ExecutionEventSpotSellDefault()           = default;
    ~ExecutionEventSpotSellDefault() override = default;
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
    ExecutionEventSpotBuyDefault()           = default;
    ~ExecutionEventSpotBuyDefault() override = default;
    double ExecQty() const override { return this->exec_qty_; }
    void Accept(aos::PositionStorageByPairInterface<double, double, PositionT>&
                    position_storage) override {
        position_storage.AddPosition(this->ExchangeId(), this->TradingPair(),
                                     this->ExecPrice(), this->ExecQty());
    }
};

template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventLinearSellDefault
    : public ExecutionEventInterface<MemoryPool, PositionT> {
  public:
    ExecutionEventLinearSellDefault()           = default;
    ~ExecutionEventLinearSellDefault() override = default;
    double ExecQty() const override {
        return -this->exec_qty_;  // minus is determine that sell for Linear
    };
    void Accept(aos::PositionStorageByPairInterface<double, double, PositionT>&
                    position_storage) override {
        position_storage.AddPosition(this->ExchangeId(), this->TradingPair(),
                                     this->ExecPrice(), this->ExecQty());
    }
};

template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventLinearBuyDefault
    : public ExecutionEventInterface<MemoryPool, PositionT> {
  public:
    ExecutionEventLinearBuyDefault()           = default;
    ~ExecutionEventLinearBuyDefault() override = default;
    double ExecQty() const override {
        return -this->exec_qty_;  // minus is determine that buy for Linear
    };
    void Accept(aos::PositionStorageByPairInterface<double, double, PositionT>&
                    position_storage) override {
        position_storage.RemovePosition(this->ExchangeId(), this->TradingPair(),
                                        this->ExecPrice(), this->ExecQty());
    }
};
};  // namespace bybit
};  // namespace aoe