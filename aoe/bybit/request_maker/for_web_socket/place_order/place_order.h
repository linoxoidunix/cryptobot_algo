#pragma once
#include <boost/pointer_cast.hpp>  // обязательно!

#include "aoe/bybit/request_maker/for_web_socket/place order/i_place_order.h"
#include "aot/common/time_utils.h"
namespace aoe {
namespace bybit {
namespace place_order {
template <template <typename> typename MemoryPool>
class RequestMaker : public RequestMakerInterface<MemoryPool> {
  public:
    nlohmann::json Make(
        boost::intrusive_ptr<aos::OrderTypeInterface<MemoryPool>> event)
        override {
        return CreateRequestJson(event);
    };

    ~RequestMaker() override = default;

  private:
    json CreateHeaderJson() {
        // TODO
        //  {"X-BAPI-RECV-WINDOW", "8000"},
        //  {"Referer", "bot-001"}
        auto timestamp = common::getCurrentNanoS();
        return {
            {"X-BAPI-TIMESTAMP", std::to_string(timestamp)},
        };
    }
    json CreateArgsEntryJson(
        boost::intrusive_ptr<aos::OrderTypeInterface<MemoryPool>> event) {
        auto derived_ptr = boost::static_pointer_cast<
            aoe::bybit::OrderTypeInterface<MemoryPool>>(event);
        auto json_object = derived_ptr->Accept(this);
        return json_object;
    }
    json CreateRequestJson() {
        auto derived_ptr = boost::static_pointer_cast<
            aoe::bybit::OrderTypeInterface<MemoryPool>>(event);
        return {{"reqId", std::to_string(derived_ptr->OrderId())},
                {"header", CreateHeaderJson()},
                {"op", "order.create"},
                {"args", json::array({CreateArgsEntryJson()})}};
    }
};
};  // namespace place_order
};  // namespace bybit
};  // namespace aoe