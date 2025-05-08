#include <boost/asio.hpp>

#include "aoe/bybit/execution_event/types.h"
#include "aoe/bybit/execution_watcher/execution_watcher.h"
#include "aoe/bybit/parser/json/ws/execution_response/execution_event_parser.h"
#include "aos/aos.h"
#include "aot/common/mem_pool.h"

struct DummyPosition {};
using PositionT = aos::impl::NetPositionDefault<double, double>;

int main() {
    using Parser =
        aoe::bybit::impl::ExecutionEventParser<common::MemoryPoolThreadSafety,
                                               PositionT>;

    std::string json = R"({
        "data": [
            { "category": "spot", "symbol": "BTCUSDT", "side": "Buy", "execFee": "0.005061", "execPrice": "0.3374", "execQty": "25", "execValue": "8.435", "orderId": "123123"}
        ]
    })";
    // std::string json = R"({
    //     "data": [
    //         { "category": "spot", "side": "Buy" },
    //         { "category": "linear", "side": "Sell" }
    //     ]
    // })";
    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json);
    auto doc = parser.iterate(padded);

    Parser event_parser(/*pool_size=*/100);
    // event_parser.RegisterFromConfig(
    //     aoe::bybit::impl::GetDefaultEventConfig<common::MemoryPoolThreadSafety,
    //                                             PositionT>());

    auto [ok, event] = event_parser.ParseAndCreate(doc.value());
    if (ok && event) {
        // event->Print();
    } else {
        // std::cout << "No matching event found.\n";
    }

    return 0;
}

// template <typename... Pools>
// class MultiTypeMemoryPoolByArgType {
//     std::unordered_map<std::type_index, std::shared_ptr<void>> pools_;

//   public:
//     template <typename PoolType, typename... Args>
//     void Emplace(Args&&... args) {
//         pools_[std::type_index(typeid(PoolType))] =
//             std::make_shared<PoolType>(std::forward<Args>(args)...);
//     }

//     template <typename PoolType>
//     std::shared_ptr<PoolType> Get() const {
//         auto it = pools_.find(std::type_index(typeid(PoolType)));
//         if (it != pools_.end()) {
//             return std::static_pointer_cast<PoolType>(it->second);
//         }
//         return nullptr;
//     }
// };

// int main() {
//     MultiTypeMemoryPoolByArgType<common::MemoryPoolThreadSafety<int>,
//                                  common::MemoryPoolThreadSafety<double>>
//         pool;
//     // pool.Emplace<PoolA>(42);
//     // pool.Emplace<PoolB>("denis");
//     pool.Emplace<common::MemoryPoolThreadSafety<int>>(1);
//     pool.Emplace<common::MemoryPoolThreadSafety<double>>(2);

//     auto pa = pool.Get<common::MemoryPoolThreadSafety<int>>();
//     auto pb = pool.Get<common::MemoryPoolThreadSafety<double>>();
//     return 0;
// }