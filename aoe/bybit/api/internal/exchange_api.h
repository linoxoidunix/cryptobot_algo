#pragma once
#include "aoe/bybit/api/i_exchange_api.h"
namespace aoe {
namespace bybit {
namespace impl {
namespace internal {
template <template <typename> typename MemoryPool>
class SingleOrderAPI : public SingleOrderAPIInterface<MemoryPool> {
  public:
    void PlaceOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
    void AmendOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
    void CancelOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
    void CancelAllOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
    ~SingleOrderAPI() override = default;
};
};  // namespace internal
};  // namespace impl
};  // namespace bybit
};  // namespace aoe