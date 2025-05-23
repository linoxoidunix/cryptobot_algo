#pragma once

namespace aoe {
namespace binance {
class RequestSenderDeafaultInterface {
  public:
    virtual void Send()                       = 0;
    virtual ~RequestSenderDeafaultInterface() = default;
};
};  // namespace binance
};  // namespace aoe