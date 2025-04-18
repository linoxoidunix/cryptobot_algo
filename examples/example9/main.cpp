#include <iostream>
#include <queue>
#include <thread>
#include <vector>

#include "aos/uid/number_pool.h"
#include "aos/uid/uid_manager.h"
#include "aos/uid/unique_id_generator.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"

int main() {
    aos::impl::UIDManagerContainer uid_manager_container(1);
    auto uid_manager = uid_manager_container.Build();
    for (int i = 0; i < 100; i++) {
        auto it = uid_manager->GetUniqueID();
        if (i % 2 == 0) {
            uid_manager->ReturnIDToPool(it);
        }
        logi("uid:{}", it);
    }
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
