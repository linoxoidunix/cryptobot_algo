#pragma once

namespace aoe {
class ResponseQueueListenerInterface {
  public:
    virtual void OnDataEnqueued()             = 0;
    virtual ~ResponseQueueListenerInterface() = default;
};
};  // namespace aoe