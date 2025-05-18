#pragma once
#include "aoe/binance/request/get_snapshot/request.h"
#include "aoe/binance/request_maker/for_rest/get_snapshot/request_maker.h"
#include "aoe/binance/session/rest/i_session.h"
#include "aoe/rest_request_sender/i_rest_request_sender.h"

namespace aoe {
namespace binance {
namespace snapshot {
namespace impl {
template <template <typename> typename MemoryPool>
class SnapshotRequestSender : public RestRequestSenderInterface<MemoryPool> {
    RestSessionRWInterface& session_;

  public:
    SnapshotRequestSender(RestSessionRWInterface& session)
        : session_(session) {}
    void Send(aos::RequestInterface<MemoryPool>& request) override {
        aoe::binance::snapshot::impl::RequestMaker<MemoryPool> request_maker;
        auto& local_req =
            static_cast<aoe::binance::snapshot::RequestInterface<MemoryPool>&>(
                request);
        auto [status, value] = local_req.Accept(&request_maker);
        if (!status) return;
        session_.AsyncWrite(std::move(value));
    }
    ~SnapshotRequestSender() override = default;
};
};  // namespace impl
};  // namespace snapshot
};  // namespace binance
};  // namespace aoe