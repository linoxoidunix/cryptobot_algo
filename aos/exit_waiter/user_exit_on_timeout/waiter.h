#pragma once
#include <atomic>
#include <csignal>
#include <future>

#include "aos/exit_waiter/i_exit_waiter.h"
#include "aos/logger/mylog.h"
#include "boost/asio/io_context.hpp"
#include "boost/asio/signal_set.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/asio/this_coro.hpp"
#include "boost/asio/thread_pool.hpp"

namespace aos {
namespace impl {
class UserExitOnTimeoutWaiter : public IExitWaiterInterface {
  public:
    UserExitOnTimeoutWaiter(boost::asio::thread_pool& pool,
                            std::chrono::steady_clock::duration timeout)
        : timer_(pool.get_executor(), timeout) {}

    void Wait() override {
        std::promise<void> promise;
        auto future = promise.get_future();

        timer_.async_wait([&](const boost::system::error_code& ec) {
            if (!ec) {
                logi("Timeout reached");
            } else {
                loge("Timer error: {}", ec.message());
            }
            promise.set_value();
        });

        future.wait();  // блокируемся здесь
    }

  private:
    boost::asio::steady_timer timer_;
};

}  // namespace impl
}  // namespace aos