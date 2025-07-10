#pragma once

namespace aos {
struct IExitWaiterInterface {
    virtual void Wait()             = 0;
    virtual ~IExitWaiterInterface() = default;
};
struct IAsyncExitWaiterInterface {
    virtual boost::asio::awaitable<void> WaitAsync() = 0;
    virtual ~IAsyncExitWaiterInterface()             = default;
};

}  // namespace aos