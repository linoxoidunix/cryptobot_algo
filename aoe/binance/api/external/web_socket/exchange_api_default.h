#pragma once
#include "aoe/binance/api/external/web_socket/exchange_api.h"
#include "aoe/binance/api/i_exchange_api.h"
#include "aoe/binance/request_maker/for_web_socket/amend_order/request_maker.h"
#include "aoe/binance/request_maker/for_web_socket/cancel_order/request_maker.h"
#include "aoe/binance/request_maker/for_web_socket/place_order/request_maker.h"
#include "aoe/session_provider/web_socket/i_web_socket_provider.h"
namespace aoe {
namespace binance {
namespace impl {
namespace external {
namespace ws {
template <template <typename> typename MemoryPool>
class SingleOrderAPIDefault : public SingleOrderAPIInterface<MemoryPool> {
    SingleOrderAPI<MemoryPool> api_;
    aoe::binance::place_order::RequestMaker<MemoryPool>
        place_order_request_maker_;
    aoe::binance::cancel_order::RequestMaker<MemoryPool>
        cancel_order_request_maker_;
    aoe::binance::amend_order::RequestMaker<MemoryPool>
        amend_order_request_maker_;

  public:
    SingleOrderAPIDefault(WebSocketSessionProviderInterface<MemoryPool>&
                              web_socket_session_provider)
        : api_(web_socket_session_provider, place_order_request_maker_,
               cancel_order_request_maker_, amend_order_request_maker_) {}
    void PlaceOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                        event) override {
        api_.PlaceOrder(event);
    }
    void AmendOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                        event) override {
        api_.AmendOrder(event);
    }
    void CancelOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                         event) override {
        api_.CancelOrder(event);
    }
    void CancelAllOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                            event) override {
        api_.CancelAllOrder(event);
    }
    ~SingleOrderAPIDefault() override = default;
};
};  // namespace ws
};  // namespace external
};  // namespace impl
};  // namespace binance
};  // namespace aoe