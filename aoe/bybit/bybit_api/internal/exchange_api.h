#pragma once
#include "aoe/bybit/bybit_api/i_exchange_api.h"
namespace aoe {
namespace bybit {
namespace impl {
namespace internal {
class SingleOrderAPI : public SingleOrderAPIInterface {
  public:
    void PlaceOrder() override {}
    void AmendOrder() override {}
    void CancelOrder() override {}
    void CancelAllOrder() override {}
    ~SingleOrderAPI() override = default;
};
};  // namespace internal
};  // namespace impl
};  // namespace bybit
};  // namespace aoe