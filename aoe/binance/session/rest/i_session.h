#pragma once
#include "aoe/session/rest/i_session.h"
#include "boost/intrusive_ptr.hpp"
#include "concurrentqueue.h"

namespace aoe {
namespace binance {

class RestAuthSessionWInterface : public RestSessionWritableInterface {
  public:
    virtual ~RestAuthSessionWInterface() = default;
};

class RestAuthSessionRWInterface : public RestSessionWritableInterface,
                                   public RestSessionReadableInterface {
  public:
    virtual ~RestAuthSessionRWInterface() = default;
};

class RestSessionWInterface : public RestSessionWritableInterface {
  public:
    virtual ~RestSessionWInterface() = default;
};

class RestSessionRWInterface : public RestSessionWritableInterface,
                               public RestSessionReadableInterface {
  public:
    virtual ~RestSessionRWInterface() = default;
};

};  // namespace binance
};  // namespace aoe