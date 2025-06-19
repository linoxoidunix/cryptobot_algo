#pragma once
#include <cstdint>

#include "aoe/bybit/request/place_order/i_request.h"
#include "aoe/bybit/request_maker/for_web_socket/place_order/i_request_maker.h"
#include "boost/pointer_cast.hpp"  // обязательно!

namespace aoe {
namespace bybit {
namespace place_order {
template <template <typename> typename MemoryPool>
class RequestMaker : public RequestMakerInterface<MemoryPool> {
  public:
    std::pair<bool, nlohmann::json> Make(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>> event)
        override {
        auto [status, value] = CreateRequestJson(event);
        return std::make_pair(status, value);
    };

    ~RequestMaker() override = default;

  private:
    nlohmann::json CreateHeaderJson() {
        int64_t timestamp = std::time(nullptr) * 1000;
        return {
            {"X-BAPI-TIMESTAMP", std::to_string(timestamp)},
        };
    }
    std::pair<bool, nlohmann::json> CreateArgsEntryJson(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>> event) {
        auto derived_ptr = boost::static_pointer_cast<
            aoe::bybit::place_order::RequestInterface<MemoryPool>>(event);
        auto [status, json_object] = derived_ptr->Accept(this);
        return std::make_pair(status, json_object);
    }
    std::pair<bool, nlohmann::json> CreateRequestJson(
        boost::intrusive_ptr<aos::RequestInterface<MemoryPool>> event) {
        // auto derived_ptr = boost::static_pointer_cast<
        //     aoe::bybit::OrderTypeInterface<MemoryPool>>(event);
        auto [status, value] = CreateArgsEntryJson(event);
        return std::make_pair(
            status, nlohmann::json{{"header", CreateHeaderJson()},
                                   {"op", "order.create"},
                                   {"args", nlohmann::json::array({value})}});
    }
};
};  // namespace place_order
};  // namespace bybit
};  // namespace aoe