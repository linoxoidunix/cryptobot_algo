#pragma once
#include "aoe/bybit/execution_event/i_types.h"
#include "boost/intrusive_ptr.hpp"
#include "simdjson.h"

namespace aoe {
namespace bybit {
template <template <typename> typename MemoryPool, typename PositionT>
class ExecutionEventParserInterface {
  public:
    virtual ~ExecutionEventParserInterface() = default;
    using EventPtr =
        boost::intrusive_ptr<ExecutionEventInterface<MemoryPool, PositionT>>;
    virtual std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) = 0;
};
};  // namespace bybit
};  // namespace aoe