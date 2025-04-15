namespace aoe {
namespace bybit {
enum class ExecType { kTrade, kUnknown };
enum class Category {
    kUnknown,
    kSpot,
    kLinear,
    kInverse,
    // kOption
};

enum class OrderType {
    kUnknown,
    kMarket,
    kLimit,
};
enum class Side {
    kUnknown,
    kSell,
    kBuy,
};
enum class StopOrderType {
    kUnknown,
    kTakeProfit,
    kStopLoss,
    kTrailingStop,
    kStop,
    kPartialTakeProfit,
    kPartialStopLoss
};
};  // namespace bybit
};  // namespace aoe