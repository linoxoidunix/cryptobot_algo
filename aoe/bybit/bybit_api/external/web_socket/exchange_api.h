#pragma once
#include "aoe/bybit/bybit_api/i_exchange_api.h"
#include "aoe/session_provider/web_socket/i_web_socket_provider.h"

namespace aoe {
namespace bybit {
namespace impl {
namespace external {
namespace ws {
template <template <typename> typename MemoryPool>
class SingleOrderAPI : public SingleOrderAPIInterface<MemoryPool> {
    WebSocketSessionProviderInterface<MemoryPool> web_socket_session_provider_;

  public:
    SingleOrderAPI(WebSocketSessionProviderInterface<MemoryPool>&
                       web_socket_session_provider,
                   place_order::RequestProviderInterface<MemoryPool>&
                       request_provider_interface)
        : web_socket_session_provider_(web_socket_session_provider) {}
    void PlaceOrder(
        boost::intrusive_ptr<OrderTypeInterface<MemoryPool>> event) override {
        auto& web_session      = web_socket_session_provider_.Provide();
        nlohmann::json request = event->ToJson();
        web_session.AsyncWrite(std::move(request));
    }
    void AmendOrder(
        boost::intrusive_ptr<OrderTypeInterface<MemoryPool>>) override {}
    void CancelOrder(
        boost::intrusive_ptr<OrderTypeInterface<MemoryPool>>) override {}
    void CancelAllOrder(
        boost::intrusive_ptr<OrderTypeInterface<MemoryPool>>) override {}
    ~SingleOrderAPI() override = default;
};
};  // namespace ws
};  // namespace external
};  // namespace impl
};  // namespace bybit
};  // namespace aoe