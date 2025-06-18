#pragma once
#include <cstdio>
#include <iostream>
#include <memory>
#include <string_view>

#include "aos/logger/mylog.h"
#include "boost/asio.hpp"
#include "boost/asio/as_tuple.hpp"
#include "boost/asio/awaitable.hpp"
#include "boost/asio/co_spawn.hpp"
#include "boost/asio/io_context.hpp"
#include "boost/asio/ssl/context.hpp"
#include "boost/system/error_code.hpp"

struct LogPolling {
    std::shared_ptr<boost::asio::steady_timer> timer;
    std::function<void(const boost::system::error_code&)> poll;
    boost::asio::cancellation_signal cancel_signal_;
    bool run_ = true;
    LogPolling(boost::asio::thread_pool& pool,
               std::chrono::microseconds interval)
        : timer(std::make_shared<boost::asio::steady_timer>(pool)) {
        cancel_signal_.slot().assign(
            [this](boost::asio::cancellation_type_t type) {
                run_ = false;
                timer->cancel();
            });
        boost::asio::co_spawn(pool, Run(interval), [](std::exception_ptr e) {
            if (e) std::rethrow_exception(e);
        });
    }

    void Stop() { cancel_signal_.emit(boost::asio::cancellation_type::all); }

  private:
    boost::asio::awaitable<void> Run(std::chrono::microseconds interval) {
        boost::system::error_code ec;
        while (run_) {
            // Wait for the timer without throwing on error
            co_await timer->async_wait(
                boost::asio::redirect_error(boost::asio::use_awaitable, ec));

            if (ec) {
                if (ec == boost::asio::error::operation_aborted) {
                    // Timer was cancelled; exit the coroutine gracefully
                    co_return;
                } else {
                    // Handle other errors if needed
                    logi("Unexpected error: {}\n", ec.message());
                    co_return;
                }
            }

            fmtlog::poll();  // Perform log polling
            timer->expires_after(interval);
        }
    }
};