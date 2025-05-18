#pragma once
#include "aos/request/i_request.h"
#include "boost/asio.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/http/message.hpp"
#include "boost/beast/version.hpp"
#include "boost/intrusive_ptr.hpp"
namespace aoe {
namespace binance {
namespace snapshot {
template <template <typename> typename MemoryPool>
class RequestMakerInterface {
  public:
    virtual std::pair<
        bool, boost::beast::http::request<boost::beast::http::string_body>>
    Make(boost::intrusive_ptr<aos::RequestInterface<MemoryPool>> event) = 0;
    virtual ~RequestMakerInterface() = default;
};
};  // namespace snapshot
};  // namespace binance
};  // namespace aoe