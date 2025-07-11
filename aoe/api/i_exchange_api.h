#pragma once
#include "aos/request/i_request.h"
#include "boost/intrusive_ptr.hpp"

namespace aoe {
template <template <typename> typename MemoryPool>
class PlaceOrderInterface {
  public:
    virtual ~PlaceOrderInterface() = default;
    virtual void PlaceOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) = 0;
};

template <template <typename> typename MemoryPool>
class AmendOrderInterface {
  public:
    virtual ~AmendOrderInterface() = default;
    virtual void AmendOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) = 0;
};

template <template <typename> typename MemoryPool>
class CancelOrderInterface {
  public:
    virtual ~CancelOrderInterface() = default;
    virtual void CancelOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) = 0;
};

template <template <typename> typename MemoryPool>
class CancelAllOrderInterface {
  public:
    virtual ~CancelAllOrderInterface() = default;
    virtual void CancelAllOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) = 0;
};

template <template <typename> typename MemoryPool>
class PlaceBatchOrderInterface {
  public:
    virtual ~PlaceBatchOrderInterface() = default;
    virtual void PlaceBatchOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) = 0;
};

template <template <typename> typename MemoryPool>
class AmendBatchOrderInterface {
  public:
    virtual ~AmendBatchOrderInterface() = default;
    virtual void AmendBatchOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) = 0;
};

template <template <typename> typename MemoryPool>
class CancelBatchOrderInterface {
  public:
    virtual ~CancelBatchOrderInterface() = default;
    virtual void CancelBatchOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) = 0;
};

template <template <typename> typename MemoryPool>
class SingleOrderAPIInterface : public PlaceOrderInterface<MemoryPool>,
                                public AmendOrderInterface<MemoryPool>,
                                public CancelOrderInterface<MemoryPool>,
                                public CancelAllOrderInterface<MemoryPool> {
  public:
    ~SingleOrderAPIInterface() override = default;
};

template <template <typename> typename MemoryPool>
class BatchOrderAPIInterface : public PlaceBatchOrderInterface<MemoryPool>,
                               public AmendBatchOrderInterface<MemoryPool>,
                               public CancelBatchOrderInterface<MemoryPool> {
  public:
    ~BatchOrderAPIInterface() override = default;
};
}  // namespace aoe
