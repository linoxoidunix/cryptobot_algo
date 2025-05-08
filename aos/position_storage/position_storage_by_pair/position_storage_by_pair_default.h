#pragma once
#include "aos/pnl/realized_calculator/pnl_realized_calculator.h"
#include "aos/pnl/realized_storage/pnl_realized_storage.h"
#include "aos/pnl/unrealized_calculator/pnl_unrealized_calculator.h"
#include "aos/pnl/unrealized_storage/pnl_unrealized_storage.h"
#include "aos/position/position.h"
#include "aos/position_storage/position_storage_by_pair/i_position_storage_by_pair.h"
#include "aos/position_storage/position_storage_by_pair/position_storage_by_pair.h"
#include "aos/position_strategy/position_strategy.h"
namespace aos {
namespace impl {

template <typename Price, typename Qty, typename PositionT, typename StrategyT,
          typename UnrealizedStorageT>
class GenericPositionStorageByPair
    : public PositionStorageByPairInterface<Price, Qty, PositionT> {
    aos::impl::RealizedPnlCalculatorDefault<Price, Qty>
        realized_pnl_calculator_;
    aos::impl::RealizedPnlStorageDefault<Price, Qty> realized_pnl_storage_;
    aos::impl::UnRealizedPnlCalculatorDefault<Price, Qty>
        unrealized_pnl_calculator_;
    UnrealizedStorageT un_realized_pnl_storage_;

    StrategyT strategy_;

    aos::impl::PositionStorageByPair<Price, Qty, PositionT> storage_;

  private:
    PositionT CreateNewPosition() { return PositionT(strategy_); }

  public:
    GenericPositionStorageByPair()
        : realized_pnl_storage_(realized_pnl_calculator_),
          un_realized_pnl_storage_(unrealized_pnl_calculator_),
          strategy_(realized_pnl_storage_, un_realized_pnl_storage_),
          storage_([this]() { return CreateNewPosition(); }) {}

    ~GenericPositionStorageByPair() override = default;

    std::optional<std::reference_wrapper<const PositionT>> GetPosition(
        common::ExchangeId exchange,
        aos::TradingPair trading_pair) const override {
        return storage_.GetPosition(exchange, trading_pair);
    }

    void AddPosition(common::ExchangeId exchange, aos::TradingPair trading_pair,
                     Price price, Qty qty) override {
        storage_.AddPosition(exchange, trading_pair, price, qty);
    }

    bool RemovePosition(common::ExchangeId exchange,
                        aos::TradingPair trading_pair, Price price,
                        Qty qty) override {
        return storage_.RemovePosition(exchange, trading_pair, price, qty);
    }
};

template <typename Price = double, typename Qty = double,
          typename PositionT = aos::impl::NetPositionDefault<Price, Qty>>
using NetPositionStorageByPairDefault = GenericPositionStorageByPair<
    Price, Qty, PositionT, aos::impl::NetPositionStrategyDefault<Price, Qty>,
    aos::impl::NetUnRealizedPnlStorageDefault<Price, Qty>>;

template <typename Price = double, typename Qty = double,
          typename PositionT = aos::impl::HedgedPositionDefault<Price, Qty>>
using HedgedPositionStorageByPairDefault = GenericPositionStorageByPair<
    Price, Qty, PositionT, aos::impl::HedgedPositionStrategyDefault<Price, Qty>,
    aos::impl::HedgedUnRealizedPnlStorageDefault<Price, Qty>>;

// template <typename Price = double, typename Qty = double,
//           typename PositionT = aos::impl::NetPositionDefault<Price, Qty>>
// class NetPositionStorageByPairDefault
//     : public PositionStorageByPairInterface<Price, Qty, PositionT> {
//     aos::impl::RealizedPnlCalculatorDefault<Price, Qty>
//         realized_pnl_calculator_;
//     aos::impl::RealizedPnlStorageDefault<Price, Qty> realized_pnl_storage_;
//     aos::impl::UnRealizedPnlCalculatorDefault<Price, Qty>
//         unrealized_pnl_calculator;
//     aos::impl::NetUnRealizedPnlStorageDefault<Price, Qty>
//         un_realized_pnl_storage_;

//     aos::impl::NetPositionStrategyDefault<Price, Qty> strategy_;

//     aos::impl::PositionStorageByPair<Price, Qty, PositionT> storage_;

//   private:
//     PositionT CreateNewPosition() { return PositionT(strategy_); }

//   public:
//     NetPositionStorageByPairDefault()
//         : realized_pnl_storage_(realized_pnl_calculator_),
//           un_realized_pnl_storage_(unrealized_pnl_calculator),
//           strategy_(realized_pnl_storage_, un_realized_pnl_storage_),
//           storage_([this]() { return CreateNewPosition(); }) {};
//     ~NetPositionStorageByPairDefault() override {};
//     std::optional<std::reference_wrapper<const PositionT>> GetPosition(
//         common::ExchangeId exchange,
//         aos::TradingPair trading_pair) const override {
//         return storage_.GetPosition(exchange, trading_pair);
//     }

//     void AddPosition(common::ExchangeId exchange,
//                      aos::TradingPair trading_pair, Price price,
//                      Qty qty) override {
//         storage_.AddPosition(exchange, trading_pair, price, qty);
//     }

//     bool RemovePosition(common::ExchangeId exchange,
//                         aos::TradingPair trading_pair, Price price,
//                         Qty qty) override {
//         return storage_.RemovePosition(exchange, trading_pair, price, qty);
//     }
// };

// template <typename Price = double, typename Qty = double,
//           typename PositionT = aos::impl::HedgedPositionDefault<Price, Qty>>
// class HedgedPositionStorageByPairDefault
//     : public PositionStorageByPairInterface<Price, Qty, PositionT> {
//     aos::impl::RealizedPnlCalculatorDefault<Price, Qty>
//         realized_pnl_calculator_;
//     aos::impl::RealizedPnlStorageDefault<Price, Qty> realized_pnl_storage_;
//     aos::impl::UnRealizedPnlCalculatorDefault<Price, Qty>
//         unrealized_pnl_calculator;
//     aos::impl::HedgedUnRealizedPnlStorageDefault<Price, Qty>
//         un_realized_pnl_storage_;

//     aos::impl::HedgedPositionStrategyDefault<Price, Qty> strategy_;

//     aos::impl::PositionStorageByPair<Price, Qty, PositionT> storage_;

//   private:
//     PositionT CreateNewPosition() { return PositionT(strategy_); }

//   public:
//     HedgedPositionStorageByPairDefault()
//         : realized_pnl_storage_(realized_pnl_calculator_),
//           un_realized_pnl_storage_(unrealized_pnl_calculator),
//           strategy_(realized_pnl_storage_, un_realized_pnl_storage_),
//           storage_([this]() { return CreateNewPosition(); }) {};
//     ~HedgedPositionStorageByPairDefault() override {};
//     std::optional<std::reference_wrapper<const PositionT>> GetPosition(
//         common::ExchangeId exchange,
//         aos::TradingPair trading_pair) const override {
//         return storage_.GetPosition(exchange, trading_pair);
//     }

//     void AddPosition(common::ExchangeId exchange,
//                      aos::TradingPair trading_pair, Price price,
//                      Qty qty) override {
//         storage_.AddPosition(exchange, trading_pair, price, qty);
//     }

//     bool RemovePosition(common::ExchangeId exchange,
//                         aos::TradingPair trading_pair, Price price,
//                         Qty qty) override {
//         return storage_.RemovePosition(exchange, trading_pair, price, qty);
//     }
// };
};  // namespace impl
};  // namespace aos
