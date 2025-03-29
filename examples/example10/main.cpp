#include <iostream>
#include <queue>
#include <thread>
#include <vector>

#include "aos/exchange/internal/binance_exchange.h"
#include "aos/order_manager/order_manager/order_manager.h"
#include "aos/order_manager/order_submitter/order_submitter.h"
#include "aos/uid/number_pool.h"
#include "aos/uid/uid_manager.h"
#include "aos/uid/unique_id_generator.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"

int main() {
    {
        using Price = double;
        using Qty   = double;
        using Uid   = size_t;
        using MyOrderSubmitter =
            aos::impl::OrderSubmitter<Price, Qty,
                                      common::MemoryPoolNotThreadSafety, Uid>;
        using MyOrderSubmitterBuilder =
            aos::impl::OrderSubmitterBuilder<MyOrderSubmitter, Price, Qty,
                                             common::MemoryPoolNotThreadSafety,
                                             Uid>;
        using MyExchangeRegistry =
            aos::impl::ExchangeRegistry<Price, Qty,
                                        common::MemoryPoolNotThreadSafety, Uid>;
        using MyExchangeRegistryBuilder = aos::impl::ExchangeRegistryBuilder<
            MyExchangeRegistry, Price, Qty, common::MemoryPoolNotThreadSafety,
            Uid>;

        fmtlog::setLogLevel(fmtlog::INF);
        boost::asio::thread_pool thread_pool;
        // start declaration containers
        aos::impl::BinanceExchangeContainer<
            Price, Qty, common::MemoryPoolNotThreadSafety, Uid>
            binance_exchange_container(1, thread_pool);

        aos::impl::UIDManagerContainer uid_manager_container(1);

        aos::impl::OrderSubmitterContainer<
            MyOrderSubmitter, MyOrderSubmitterBuilder, MyExchangeRegistry,
            MyExchangeRegistryBuilder, Price, Qty,
            common::MemoryPoolNotThreadSafety, Uid>
            order_submit_container(1);
        // stop declaration containers
        auto uid_manager_ptr = uid_manager_container.Build();

        auto [order_submitter_ptr, exchange_registry_ptr] =
            order_submit_container.Build();
        aos::impl::OrderManager<double, double, size_t> order_manager(
            order_submitter_ptr, uid_manager_ptr);

        auto binance_exchange_ptr = binance_exchange_container.Build();

        binance_exchange_ptr->Subscribe(&order_manager);
        exchange_registry_ptr->RegisterExchange(aos::OrderRouting::Internal,
                                                common::ExchangeId::kBinance,
                                                binance_exchange_ptr);

        order_manager.PlaceOrder(common::ExchangeId::kBinance, {2, 1}, 100, 10,
                                 common::Side::kAsk, aos::TimeInForce::GTC,
                                 aos::OrderRouting::Internal);
        order_manager.CancelOrder(common::ExchangeId::kBinance,
                                  aos::OrderRouting::Internal, 0);
    }
    fmtlog::poll();
    int x = 0;
    return 0;
}

// #include <boost/asio.hpp>
// #include <chrono>
// #include <iostream>
// #include <thread>

// void Task1(
//     boost::asio::strand<boost::asio::thread_pool::executor_type>& strand1,
//     int id) {
//     boost::asio::post(strand1, [id]() {
//         std::cout << "Task1: " << id << " running on thread "
//                   << std::this_thread::get_id() << "\n";
//         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     });
// }

// void Task2(
//     boost::asio::strand<boost::asio::thread_pool::executor_type>& strand2,
//     int id) {
//     boost::asio::post(strand2, [id]() {
//         std::cout << "Task2: " << id << " running on thread "
//                   << std::this_thread::get_id() << "\n";
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     });
// }

// int main() {
//     boost::asio::thread_pool pool(4);  // Потоковый пул из 4 потоков

//     boost::asio::strand<boost::asio::thread_pool::executor_type> strand1(
//         pool.get_executor());
//     boost::asio::strand<boost::asio::thread_pool::executor_type> strand2(
//         pool.get_executor());

//     for (int i = 0; i < 20; ++i) {
//         Task1(strand1, i);
//         Task2(strand2, i);
//     }

//     pool.join();  // Ожидаем завершения всех задач
//     return 0;
// }

// #include <boost/asio.hpp>
// #include <iostream>
// #include <unordered_map>

// // Пример задачи
// void Task1(boost::asio::strand<boost::asio::thread_pool::executor_type>&
// strand,
//            int id) {
//     boost::asio::post(strand, [id]() {
//         std::cout << "Task1 executing with ID: " << id << std::endl;
//     });
// }

// void Task2(boost::asio::strand<boost::asio::thread_pool::executor_type>&
// strand,
//            int id) {
//     boost::asio::post(strand, [id]() {
//         std::cout << "Task2 executing with ID: " << id << std::endl;
//     });
// }

// int main() {
//     boost::asio::thread_pool pool(4);  // Потоковый пул из 4 потоков

//     // Контейнер для хранения strand по ключу
//     std::unordered_map<
//         size_t, boost::asio::strand<boost::asio::thread_pool::executor_type>>
//         map;
//     // Создаем два strand и сохраняем их в контейнер
//     auto strand1 = boost::asio::make_strand(pool.get_executor());
//     auto strand2 = boost::asio::make_strand(pool.get_executor());
//     map.insert_or_assign(1, strand1);
//     map.insert_or_assign(2, strand2);
//     // Запускаем задачи с использованием strand
//     for (int i = 0; i < 20; ++i) {
//         Task1(map.at(1), i);  // Используем strand 1
//         Task2(map.at(2), i);  // Используем strand 2
//     }

//     pool.join();  // Ждем завершения всех задач
//     return 0;
// }
