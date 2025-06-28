#pragma once

#include "aoe/api/i_exchange_api.h"

namespace aoe::impl {

template <template <typename> typename MemoryPool>
class PlaceOrderDummy : public aoe::PlaceOrderInterface<MemoryPool> {
  public:
    ~PlaceOrderDummy() override = default;

    /**
     * @brief Dummy implementation — does nothing.
     *
     * This method is intentionally left empty as this is a stub implementation
     * of the PlaceOrderInterface, used for testing or fallback logic.
     */
    void PlaceOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
};

template <template <typename> typename MemoryPool>
class AmendOrderDummy : public aoe::AmendOrderInterface<MemoryPool> {
  public:
    ~AmendOrderDummy() override = default;

    /**
     * @brief Dummy implementation — does nothing.
     *
     * This method is intentionally left empty as this is a stub implementation
     * of the AmendOrderInterface, used for testing or fallback logic.
     */
    void AmendOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
};

template <template <typename> typename MemoryPool>
class CancelOrderDummy : public aoe::CancelOrderInterface<MemoryPool> {
  public:
    ~CancelOrderDummy() override = default;

    /**
     * @brief Dummy implementation — does nothing.
     *
     * This method is intentionally left empty as this is a stub implementation
     * of the CancelOrderInterface, used for testing or fallback logic.
     */
    void CancelOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
};

template <template <typename> typename MemoryPool>
class CancelAllOrderDummy : public aoe::CancelAllOrderInterface<MemoryPool> {
  public:
    ~CancelAllOrderDummy() override = default;

    /**
     * @brief Dummy implementation — does nothing.
     *
     * This method is intentionally left empty as this is a stub implementation
     * of the CancelAllOrderInterface, used for testing or fallback logic.
     */
    void CancelAllOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
};

template <template <typename> typename MemoryPool>
class PlaceBatchOrderDummy : public aoe::PlaceBatchOrderInterface<MemoryPool> {
  public:
    ~PlaceBatchOrderDummy() override = default;

    /**
     * @brief Dummy implementation — does nothing.
     *
     * This method is intentionally left empty as this is a stub implementation
     * of the PlaceBatchOrderInterface, used for testing or fallback logic.
     */
    void PlaceBatchOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
};

template <template <typename> typename MemoryPool>
class AmendBatchOrderDummy : public aoe::AmendBatchOrderInterface<MemoryPool> {
  public:
    ~AmendBatchOrderDummy() override = default;

    /**
     * @brief Dummy implementation — does nothing.
     *
     * This method is intentionally left empty as this is a stub implementation
     * of the AmendBatchOrderInterface, used for testing or fallback logic.
     */
    void AmendBatchOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
};

template <template <typename> typename MemoryPool>
class CancelBatchOrderDummy
    : public aoe::CancelBatchOrderInterface<MemoryPool> {
  public:
    ~CancelBatchOrderDummy() override = default;

    /**
     * @brief Dummy implementation — does nothing.
     *
     * This method is intentionally left empty as this is a stub implementation
     * of the CancelBatchOrderInterface, used for testing or fallback logic.
     */
    void CancelBatchOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
};

template <template <typename> typename MemoryPool>
class SingleOrderAPIDummy : public aoe::SingleOrderAPIInterface<MemoryPool> {
  public:
    ~SingleOrderAPIDummy() override = default;

    /**
     * @brief Dummy implementation — does nothing.
     */
    void PlaceOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}

    /**
     * @brief Dummy implementation — does nothing.
     */
    void AmendOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}

    /**
     * @brief Dummy implementation — does nothing.
     */
    void CancelOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}

    /**
     * @brief Dummy implementation — does nothing.
     */
    void CancelAllOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
};

template <template <typename> typename MemoryPool>
class BatchOrderAPIDummy : public aoe::BatchOrderAPIInterface<MemoryPool> {
  public:
    ~BatchOrderAPIDummy() override = default;

    /**
     * @brief Dummy implementation — does nothing.
     */
    void PlaceBatchOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}

    /**
     * @brief Dummy implementation — does nothing.
     */
    void AmendBatchOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}

    /**
     * @brief Dummy implementation — does nothing.
     */
    void CancelBatchOrder(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>) override {}
};

}  // namespace aoe::impl