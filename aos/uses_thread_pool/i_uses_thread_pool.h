#include "boost/asio.hpp"

namespace aos {
class UsesThreadPool {
  public:
    virtual void SetThreadPool(boost::asio::thread_pool& thread_pool) = 0;
    virtual ~UsesThreadPool()                                         = default;
};
};  // namespace aos