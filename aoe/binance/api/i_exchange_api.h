#pragma once
#include "aoe/api/i_exchange_api.h"
// #include "aos/request/i_request.h"
// #include "boost/intrusive_ptr.hpp"

namespace aoe {
namespace binance {

using aoe::AmendBatchOrderInterface;
using aoe::AmendOrderInterface;
using aoe::BatchOrderAPIInterface;
using aoe::CancelAllOrderInterface;
using aoe::CancelBatchOrderInterface;
using aoe::CancelOrderInterface;
using aoe::PlaceBatchOrderInterface;
using aoe::PlaceOrderInterface;
using aoe::SingleOrderAPIInterface;
//--------------------------------------------------------------------------
template <template <typename> typename MemoryPool>
class SnapshotRequesterInterface {
  public:
    virtual ~SnapshotRequesterInterface() = default;

    // Асинхронный или синхронный метод для запроса snapshot-а
    virtual void RequestSnapshot(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>> request) = 0;
};
};  // namespace binance
};  // namespace aoe