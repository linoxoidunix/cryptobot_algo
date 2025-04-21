#pragma once
#include "aoe/bybit/bybit_api/i_exchange_api.h"
#include "aoe/bybit/request_maker/for_web_socket/place_order/i_request_maker.h"
#include "aoe/session_provider/web_socket/i_web_socket_provider.h"
namespace aoe {
namespace bybit {
namespace impl {
namespace external {
namespace ws {
template <template <typename> typename MemoryPool>
class SingleOrderAPI : public SingleOrderAPIInterface<MemoryPool> {
    WebSocketSessionProviderInterface<MemoryPool> web_socket_session_provider_;
    aoe::bybit::place_order::RequestMakerInterface<MemoryPool>
        place_order_request_maker_;

  public:
    SingleOrderAPI(WebSocketSessionProviderInterface<MemoryPool>&
                       web_socket_session_provider,
                   aoe::bybit::place_order::RequestMakerInterface<MemoryPool>&
                       place_order_request_maker_)
        : web_socket_session_provider_(web_socket_session_provider),
          place_order_request_maker_(place_order_request_maker_) {}
    void PlaceOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                        event) override {
        auto& web_session      = web_socket_session_provider_.Provide();
        nlohmann::json request = place_order_request_maker_->Make(event);
        web_session.AsyncWrite(std::move(request));
    }
    void AmendOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                        event) override {
        auto& web_session      = web_socket_session_provider_.Provide();
        nlohmann::json request = event->ToJson();
        web_session.AsyncWrite(std::move(request));
    }
    void CancelOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                         event) override {
        auto& web_session      = web_socket_session_provider_.Provide();
        nlohmann::json request = event->ToJson();
        web_session.AsyncWrite(std::move(request));
    }
    void CancelAllOrder(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>>
                            event) override {}
    ~SingleOrderAPI() override = default;
};
};  // namespace ws
};  // namespace external
};  // namespace impl
};  // namespace bybit
};  // namespace aoe