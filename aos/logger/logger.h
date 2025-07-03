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
    boost::asio::cancellation_signal cancel_signal_;
    std::future<void> done_;  // используем future для ожидания завершения
    std::atomic_bool run_ = true;

    LogPolling(boost::asio::thread_pool& pool,
               std::chrono::microseconds interval)
        : timer(std::make_shared<boost::asio::steady_timer>(pool)) {
        cancel_signal_.slot().assign([this](boost::asio::cancellation_type_t) {
            run_ = false;
            timer->cancel();
        });

        done_ = boost::asio::co_spawn(
            pool,
            [this, interval]() -> boost::asio::awaitable<void> {
                try {
                    co_await Run(interval);
                } catch (const std::exception& e) {
                    loge("LogPolling error: {}", e.what());
                }
            },
            boost::asio::use_future  // Позволяет ожидать завершения
        );
    }

    void Stop() { cancel_signal_.emit(boost::asio::cancellation_type::all); }

    ~LogPolling() {
        Stop();
        // Ждём завершения корутины
        if (done_.valid()) {
            try {
                done_.get();  // дождаться завершения или поймать ошибку
            } catch (const std::exception& e) {
                loge("LogPolling::~LogPolling error: {}", e.what());
            }
        }
    }

  private:
    boost::asio::awaitable<void> Run(std::chrono::microseconds interval) {
        boost::system::error_code ec;
        while (run_) {
            co_await timer->async_wait(
                boost::asio::redirect_error(boost::asio::use_awaitable, ec));

            if (ec) {
                if (ec == boost::asio::error::operation_aborted) {
                    co_return;
                } else {
                    logi("LogPolling unexpected timer error: {}", ec.message());
                    co_return;
                }
            }

            fmtlog::poll();  // логгирование
            timer->expires_after(interval);
        }
    }
};
