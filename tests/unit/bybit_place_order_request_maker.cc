#include <gtest/gtest.h>

#include "aoe/aoe.h"  // Ваш класс
#include "aos/aos.h"  // Ваш класс
#include "aot/common/mem_pool.h"
#include "gmock/gmock.h"
#include "nlohmann/json.hpp"
using ::testing::Return;

using namespace aoe;
using namespace aoe::bybit;
using namespace aoe::bybit::impl;
using namespace aos;

// мок для OrderTypeInterface
template <template <typename> typename MemoryPool>
class MockOrder : public aoe::bybit::OrderTypeInterface<MemoryPool> {
  public:
    MOCK_METHOD((std::pair<bool, nlohmann::json>), Accept,
                (aoe::bybit::place_order::RequestMakerInterface<MemoryPool>*),
                (override));

    MOCK_METHOD(uint64_t, OrderId, (), (const, override));
};

TEST(BybitPlaceOrderRequestMakerTest, MakeReturnsValidRequestJson) {
    using RequestMaker =
        aoe::bybit::place_order::RequestMaker<common::MemoryPoolThreadSafety>;
    using MockOrderT = MockOrder<common::MemoryPoolThreadSafety>;

    auto mock_order  = boost::intrusive_ptr<MockOrderT>(new MockOrderT());

    // Настройка моков
    EXPECT_CALL(*mock_order, Accept(::testing::_))
        .WillOnce(Return(
            std::make_pair(true, nlohmann::json{{"category", "spot"},
                                                {"symbol", "BTCUSDT"},
                                                {"side", "Buy"},
                                                {"orderType", "Limit"},
                                                {"qty", "1"},
                                                {"orderLinkId", "123456789"},
                                                {"price", "20000"}})));

    EXPECT_CALL(*mock_order, OrderId()).WillOnce(Return(123456789));

    RequestMaker maker;
    auto [status, result] = maker.Make(mock_order);
    ASSERT_TRUE(status);
    // Проверка ключей верхнего уровня
    ASSERT_TRUE(result.contains("reqId"));
    ASSERT_TRUE(result.contains("header"));
    ASSERT_TRUE(result.contains("op"));
    ASSERT_TRUE(result.contains("args"));

    // Проверка значений
    EXPECT_EQ(result["op"], "order.create");

    // Проверка header
    ASSERT_TRUE(result["header"].contains("X-BAPI-TIMESTAMP"));
    EXPECT_FALSE(
        result["header"]["X-BAPI-TIMESTAMP"].get<std::string>().empty());

    // Проверка args
    ASSERT_TRUE(result["args"].is_array());
    ASSERT_EQ(result["args"].size(), 1);
    EXPECT_EQ(result["args"][0]["symbol"], "BTCUSDT");
    EXPECT_EQ(result["args"][0]["price"], "20000");
    EXPECT_EQ(result["args"][0]["orderLinkId"], "123456789");

    mock_order
        ->~MockOrderT();  // ❌ ПЛОХО: нарушает управление памятью intrusive_ptr
    // mock_order.reset();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}