#pragma once
namespace aoe {
namespace bybit {
class PlaceOrderInterface {
  public:
    virtual ~PlaceOrderInterface() = default;
    virtual void PlaceOrder()      = 0;
};

class AmendOrderInterface {
  public:
    virtual ~AmendOrderInterface() = default;
    virtual void AmendOrder()      = 0;
};

class CancelOrderInterface {
  public:
    virtual ~CancelOrderInterface() = default;
    virtual void CancelOrder()      = 0;
};

class CancelAllOrderInterface {
  public:
    virtual ~CancelAllOrderInterface() = default;
    virtual void CancelAllOrder()      = 0;
};

class PlaceBacthOrderInterface {
  public:
    virtual ~PlaceBacthOrderInterface() = default;
    virtual void PlaceBatchOrder()      = 0;
};

class AmendBatchOrderInterface {
  public:
    virtual ~AmendBatchOrderInterface() = default;
    virtual void AmendBatchOrder()      = 0;
};

class CancelBatchOrderInterface {
  public:
    virtual ~CancelBatchOrderInterface() = default;
    virtual void CancelBatchOrder()      = 0;
};

class SingleOrderAPIInterface : public PlaceOrderInterface,
                                public AmendOrderInterface,
                                public CancelOrderInterface,
                                public CancelAllOrderInterface {
  public:
    virtual ~SingleOrderAPIInterface() = default;
};

class BatchOrderAPIInterface : public PlaceBacthOrderInterface,
                               public AmendBatchOrderInterface,
                               public CancelBatchOrderInterface {
  public:
    virtual ~BatchOrderAPIInterface() = default;
};
};  // namespace bybit
};  // namespace aoe