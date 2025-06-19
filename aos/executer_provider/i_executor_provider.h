#pragma once
#include "aos/common/ref_counted.h"
#include "boost/asio.hpp"

namespace aos {
template <typename HashT, template <typename> class MemoryPool>
class IExecutorProvider
    : public common::RefCounted<MemoryPool,
                                aos::IExecutorProvider<HashT, MemoryPool>> {
  public:
    virtual boost::asio::strand<boost::asio::thread_pool::executor_type>&
    GetStrand(const HashT& asset) = 0;
    virtual ~IExecutorProvider()  = default;
};

}  // namespace aos