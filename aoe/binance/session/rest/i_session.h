#pragma once
#include "aoe/session/rest/i_session.h"
#include "boost/intrusive_ptr.hpp"
#include "concurrentqueue.h"

namespace aoe {
namespace binance {

class RestAuthSessionWInterface : public RestSessionWritableInterface {
  public:
    ~RestAuthSessionWInterface() override = default;
};

class RestAuthSessionRWInterface : public RestSessionWritableInterface,
                                   public RestSessionReadableInterface {
  public:
    ~RestAuthSessionRWInterface() override = default;
};

class RestSessionWInterface : public RestSessionWritableInterface {
  public:
    ~RestSessionWInterface() override = default;
};

class RestSessionRWInterface : public RestSessionWritableInterface,
                               public RestSessionReadableInterface {
  public:
    virtual void Start()               = 0;
    ~RestSessionRWInterface() override = default;
};

};  // namespace binance
};  // namespace aoe