#pragma once
#include <functional>
#include <list>
#include <unordered_map>
#include <vector>

#include "aoe/binance/order_book_event/order_book_event.h"
#include "aoe/binance/parser/json/ws/diff_response/i_parser.h"
#include "aos/converters/big_string_view_to_trading_pair/big_string_view_to_trading_pair.h"
#include "aos/order_book_level_raw/order_book_level_raw.h"

namespace aoe {
namespace binance {

namespace details {

// Common base class for diff event parsing
template <typename Price, typename Qty, template <typename> typename MemoryPool,
          typename DiffEventType>
class DiffEventParserBase
    : public DiffEventParserInterface<Price, Qty, MemoryPool, DiffEventType> {
    using EventPtr  = typename DiffEventParserInterface<Price, Qty, MemoryPool,
                                                        DiffEventType>::EventPtr;
    using FactoryFn = std::function<EventPtr()>;

  protected:
    FactoryFn factory_event_;
    MemoryPool<DiffEventType> pool_order_diff_;

  public:
    ~DiffEventParserBase() override = default;

    explicit DiffEventParserBase(std::size_t pool_size)
        : pool_order_diff_(pool_size) {
        RegisterFromConfig();
    }

    std::pair<bool, EventPtr> ParseAndCreate(
        simdjson::ondemand::document& doc) override {
        // Validate event type
        std::string_view event_type;
        if (doc["e"].get_string().get(event_type) != simdjson::SUCCESS ||
            event_type != "depthUpdate") {
            return {false, nullptr};
        }
        // // NEED PARSE E
        // --- 🔽 Парсим поле "E" (event time)
        uint64_t event_time = 0;
        if (doc["E"].get_uint64().get(event_time) != simdjson::SUCCESS) {
            return {false, nullptr};
        }
        // --- 🔽 Получаем текущее время (в мс с эпохи)
        uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();

        // --- 🔽 Вычисляем задержку
        int64_t delay_ms =
            static_cast<int64_t>(now) - static_cast<int64_t>(event_time);

        // --- 🔽 Логгируем
        logi("[depthUpdate binance] Delay: {}", delay_ms);

        EventPtr ptr = factory_event_();

        // Parse common fields
        if (!ParseCommonFields(doc, ptr)) {
            return {false, nullptr};
        }

        // Parse futures-specific field if needed
        if constexpr (requires { ptr->SetPreviousUpdateId(0); }) {
            auto pu_result = doc["pu"].get_uint64();
            if (pu_result.error() != simdjson::SUCCESS) {
                return {false, nullptr};
            }
            ptr->SetPreviousUpdateId(pu_result.value());
        }

        // Parse trading pair
        if (!ParseTradingPair(doc, ptr)) {
            return {false, nullptr};
        }

        // Parse order book levels
        if (!ParseOrderBookLevels(
                doc, "b", ptr,
                &OrderBookEventInterface<Price, Qty, MemoryPool>::SetBids) ||
            !ParseOrderBookLevels(
                doc, "a", ptr,
                &OrderBookEventInterface<Price, Qty, MemoryPool>::SetAsks)) {
            return {false, nullptr};
        }

        return {true, ptr};
    }

  private:
    void RegisterFromConfig() {
        factory_event_ = [this]() {
            auto ptr = pool_order_diff_.Allocate();
            ptr->SetMemoryPool(&pool_order_diff_);
            return ptr;
        };
    }

    bool ParseCommonFields(simdjson::ondemand::document& doc, EventPtr ptr) {
        auto u_result = doc["u"].get_uint64();
        if (u_result.error() != simdjson::SUCCESS) {
            return false;
        }
        ptr->SetFinalUpdateId(u_result.value());

        auto U_result = doc["U"].get_uint64();  // NOLINT
        if (U_result.error() != simdjson::SUCCESS) {
            return false;
        }
        ptr->SetFirstUpdateId(U_result.value());

        return true;
    }

    bool ParseTradingPair(simdjson::ondemand::document& doc, EventPtr ptr) {
        simdjson::simdjson_result<std::string_view> symbol_result =
            doc["s"].get_string();
        if (symbol_result.error() != simdjson::SUCCESS) {
            return false;
        }

        auto [status, trading_pair] =
            aos::impl::BigStringViewToTradingPair::Convert(
                symbol_result.value());
        if (!status) {
            return false;
        }

        ptr->SetTradingPair(trading_pair);
        logd("raw={} {}", symbol_result.value(), trading_pair);
        return true;
    }

    bool ParseOrderBookLevels(
        simdjson::ondemand::document& doc, const char* side, EventPtr ptr,
        void (OrderBookEventInterface<Price, Qty, MemoryPool>::*setter)(
            std::vector<aos::OrderBookLevelRaw<Price, Qty>>&&)) {
        auto levels_result = doc[side].get_array();
        if (levels_result.error() != simdjson::SUCCESS) {
            return true;  // Not an error if field is missing
        }

        std::vector<aos::OrderBookLevelRaw<Price, Qty>> levels;
        levels.reserve(1000);

        for (auto entry : levels_result.value()) {
            simdjson::ondemand::array level_pair = entry.get_array();
            auto it                              = level_pair.begin();

            auto price_result                    = (*it).get_double_in_string();
            if (price_result.error() != simdjson::SUCCESS) {
                return false;
            }
            ++it;

            auto qty_result = (*it).get_double_in_string();
            if (qty_result.error() != simdjson::SUCCESS) {
                return false;
            }

            levels.emplace_back(price_result.value(), qty_result.value());
        }

        (ptr.get()->*setter)(std::move(levels));
        return true;
    }
};

}  // namespace details

namespace spot {
namespace impl {

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class DiffEventParser
    : public aoe::binance::details::DiffEventParserBase<
          Price, Qty, MemoryPool,
          aoe::binance::impl::spot::OrderBookDiffEventDefault<Price, Qty,
                                                              MemoryPool>> {
  public:
    using Base = aoe::binance::details::DiffEventParserBase<
        Price, Qty, MemoryPool,
        aoe::binance::impl::spot::OrderBookDiffEventDefault<Price, Qty,
                                                            MemoryPool>>;

    explicit DiffEventParser(std::size_t pool_size) : Base(pool_size) {}
};

}  // namespace impl
}  // namespace spot

namespace futures {
namespace impl {

template <typename Price, typename Qty, template <typename> typename MemoryPool>
class DiffEventParser
    : public aoe::binance::details::DiffEventParserBase<
          Price, Qty, MemoryPool,
          aoe::binance::impl::futures::OrderBookDiffEventDefault<Price, Qty,
                                                                 MemoryPool>> {
  public:
    using Base = aoe::binance::details::DiffEventParserBase<
        Price, Qty, MemoryPool,
        aoe::binance::impl::futures::OrderBookDiffEventDefault<Price, Qty,
                                                               MemoryPool>>;

    explicit DiffEventParser(std::size_t pool_size) : Base(pool_size) {}
};

}  // namespace impl
}  // namespace futures

}  // namespace binance
}  // namespace aoe