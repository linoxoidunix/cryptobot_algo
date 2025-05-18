#pragma once
#include "boost/asio.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/http/message.hpp"
#include "boost/beast/version.hpp"
#include "boost/intrusive_ptr.hpp"
#include "concurrentqueue.h"

namespace aoe {

class RestSessionWritableInterface {
  public:
    virtual void AsyncWrite(
        boost::beast::http::request<boost::beast::http::string_body>&&) = 0;
    virtual ~RestSessionWritableInterface() = default;
};

class RestSessionReadableInterface {
  public:
    virtual moodycamel::ConcurrentQueue<std::vector<char>>&
    GetResponseQueue()                      = 0;
    virtual ~RestSessionReadableInterface() = default;
};
};  // namespace aoe