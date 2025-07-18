#pragma once

#include "aos/common/exchange_id.h"
#include "aos/common/ref_counted.h"
#include "aos/trading_pair/trading_pair.h"
namespace aos {

template <typename Price, typename Qty>
class NetPositionStrategyInterface {
  public:
    // Статическая проверка на поддерживаемость умножения
    static_assert(std::is_arithmetic_v<Price>,
                  "Price must be an arithmetic type");
    static_assert(std::is_arithmetic_v<Qty>, "Qty must be an arithmetic type");

    virtual ~NetPositionStrategyInterface()                       = default;
    virtual void Add(common::ExchangeId exchange_id,
                     aos::TradingPair trading_pair, Price& avg_price,
                     Qty& net_qty, Price price, Qty qty) const    = 0;
    virtual void Remove(common::ExchangeId exchange_id,
                        aos::TradingPair trading_pair, Price& avg_price,
                        Qty& net_qty, Price price, Qty qty) const = 0;
};

template <typename Price, typename Qty>
class HedgePositionStrategyInterface {
  public:
    // Статическая проверка на поддерживаемость умножения
    static_assert(std::is_arithmetic_v<Price>,
                  "Price must be an arithmetic type");
    static_assert(std::is_arithmetic_v<Qty>, "Qty must be an arithmetic type");

    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    virtual ~HedgePositionStrategyInterface()                       = default;
    virtual void Add(common::ExchangeId exchange_id,
                     aos::TradingPair trading_pair, Price (&avg_price)[2],
                     Qty (&net_qty)[2], Price price, Qty qty) const = 0;
    virtual void Remove(common::ExchangeId exchange_id,
                        aos::TradingPair trading_pair, Price (&avg_price)[2],
                        Qty (&net_qty)[2], Price price, Qty qty) const = 0;
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class INetPositionStrategy
    : public common::RefCounted<MemoryPool,
                                INetPositionStrategy<Price, Qty, MemoryPool>> {
  public:
    // Статическая проверка на поддерживаемость умножения
    static_assert(std::is_arithmetic_v<Price>,
                  "Price must be an arithmetic type");
    static_assert(std::is_arithmetic_v<Qty>, "Qty must be an arithmetic type");

    ~INetPositionStrategy() override                              = default;
    virtual void Add(common::ExchangeId exchange_id,
                     aos::TradingPair trading_pair, Price& avg_price,
                     Qty& net_qty, Price price, Qty qty) const    = 0;
    virtual void Remove(common::ExchangeId exchange_id,
                        aos::TradingPair trading_pair, Price& avg_price,
                        Qty& net_qty, Price price, Qty qty) const = 0;
};

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class IHedgePositionStrategy
    : public common::RefCounted<
          MemoryPool, IHedgePositionStrategy<Price, Qty, MemoryPool>> {
  public:
    // Статическая проверка на поддерживаемость умножения
    static_assert(std::is_arithmetic_v<Price>,
                  "Price must be an arithmetic type");
    static_assert(std::is_arithmetic_v<Qty>, "Qty must be an arithmetic type");

    using RealizedPnl = decltype(std::declval<Price>() * std::declval<Qty>());
    ~IHedgePositionStrategy() override                              = default;
    virtual void Add(common::ExchangeId exchange_id,
                     aos::TradingPair trading_pair, Price (&avg_price)[2],
                     Qty (&net_qty)[2], Price price, Qty qty) const = 0;
    virtual void Remove(common::ExchangeId exchange_id,
                        aos::TradingPair trading_pair, Price (&avg_price)[2],
                        Qty (&net_qty)[2], Price price, Qty qty) const = 0;
};
};  // namespace aos