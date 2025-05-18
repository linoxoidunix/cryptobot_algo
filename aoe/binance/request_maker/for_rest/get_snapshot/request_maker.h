#pragma once
#include <boost/pointer_cast.hpp>  // обязательно!

#include "aoe/binance/request/get_snapshot/i_request.h"
#include "aoe/binance/request_maker/for_rest/get_snapshot/i_request_maker.h"
#include "aot/common/time_utils.h"
namespace aoe {
namespace binance {
namespace snapshot {
namespace impl {
template <template <typename> typename MemoryPool>
class RequestMaker : public RequestMakerInterface<MemoryPool> {
  public:
    std::pair<bool,
              boost::beast::http::request<boost::beast::http::string_body>>
    Make(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>> event)
        override {
        auto derived_ptr = boost::static_pointer_cast<
            aoe::binance::snapshot::RequestInterface<MemoryPool>>(event);

        auto [status, value] = derived_ptr->Accept(this);
        return std::make_pair(status, value);
    };

    ~RequestMaker() override = default;
};
};  // namespace impl
};  // namespace snapshot
};  // namespace binance
};  // namespace aoe