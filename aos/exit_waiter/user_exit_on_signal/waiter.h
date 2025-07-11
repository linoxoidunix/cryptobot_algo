#pragma once
#include <atomic>
#include <csignal>
#include <future>

#include "aos/exit_waiter/i_exit_waiter.h"
#include "aos/logger/mylog.h"
#include "boost/asio/io_context.hpp"
#include "boost/asio/signal_set.hpp"
#include "boost/asio/this_coro.hpp"
#include "boost/asio/thread_pool.hpp"

namespace aos {
namespace impl {
class UserExitOnSignalWaiter : public IExitWaiterInterface {
  public:
    explicit UserExitOnSignalWaiter(boost::asio::thread_pool& pool)
        : signal_set_(pool.get_executor(), SIGINT, SIGTERM) {}

    void Wait() override {
        std::promise<void> promise;
        auto future = promise.get_future();

        signal_set_.async_wait([&](const boost::system::error_code&,
                                   int signal_number) { promise.set_value(); });

        future.wait();  // блокируемся здесь
    }

  private:
    boost::asio::signal_set signal_set_;
};

};  // namespace impl
};  // namespace aos