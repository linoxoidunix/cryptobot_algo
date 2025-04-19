#pragma once
#include "aoe/bybit/bybit_api/i_exchange_api.h"
namespace aoe {
namespace bybit {
namespace impl {
namespace external {
namespace rest {
template <template <typename> typename MemoryPool>
class SingleOrderAPI : public SingleOrderAPIInterface<MemoryPool> {
  public:
    SingleOrderAPI() {}
    void PlaceOrder(
        boost::intrusive_ptr<aos::OrderTypeInterface<MemoryPool>>) override {}
    void AmendOrder(
        boost::intrusive_ptr<aos::OrderTypeInterface<MemoryPool>>) override {}
    void CancelOrder(
        boost::intrusive_ptr<aos::OrderTypeInterface<MemoryPool>>) override {}
    void CancelAllOrder(
        boost::intrusive_ptr<aos::OrderTypeInterface<MemoryPool>>) override {}
    ~SingleOrderAPI() override = default;
};
};  // namespace rest
};  // namespace external
};  // namespace impl
};  // namespace bybit
};  // namespace aoe