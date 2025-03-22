#pragma once

namespace common {
struct IWaitable {
    virtual void Wait()  = 0;
    virtual ~IWaitable() = default;
};
};  // namespace common