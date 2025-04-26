#pragma once
#include "aoe/bybit/session/web_socket/i_web_socket.h"
#include "aoe/session/web_socket/i_web_socket.h"
#include "aoe/subscription_builder/i_subscription_builder.h"

namespace aoe {
namespace bybit {
namespace impl {
class ExecutionSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPrivateSessionInterface& ws_;

  public:
    ExecutionSubscriptionBuilder(WebSocketPrivateSessionInterface& ws)
        : ws_(ws) {}
    ~ExecutionSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json j_;
        j_["op"]   = "subscribe";
        j_["args"] = {"execution"};
        ws_.AsyncWrite(std::move(j_));
    }
};
class OrderSubscriptionBuilder : public SubscriptionBuilderInterface {
    WebSocketPrivateSessionInterface& ws_;

  public:
    OrderSubscriptionBuilder(WebSocketPrivateSessionInterface& ws) : ws_(ws) {}
    ~OrderSubscriptionBuilder() override = default;
    void Subscribe() override {
        nlohmann::json j_;
        j_["op"]   = "subscribe";
        j_["args"] = {"order"};
        ws_.AsyncWrite(std::move(j_));
    }
};
};  // namespace impl
};  // namespace bybit
};  // namespace aoe
