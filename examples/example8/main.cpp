#include <iostream>
#include <queue>
#include <thread>
#include <vector>

#include "aos/avg_tracker/avg_tracker.h"
#include "aos/deviation_tracker/deviation_tracker.h"
#include "aos/histogram/histogram_calculator.h"
#include "aos/joint_histogram/joint_histogram_calculator.h"
#include "aos/market_triplet_manager/market_triplet_manager.h"
#include "aos/max_tracker/max_tracker.h"
#include "aos/min_tracker/min_tracker.h"
#include "aos/mutual_information/mutual_information_calculator.h"
#include "aos/mutual_information_real_time/mutual_information_real_time.h"
#include "aos/sliding_window_storage/sliding_window_storage.h"
#include "aot/bus/bus.h"
#include "aot/common/mem_pool.h"
#include "aot/strategy/market_order_book.h"

std::atomic<size_t> current_index{0};

// Функция-обработчик данных
void ProcessData(size_t index, size_t hash_first_asset, double first_value,
                 size_t hash_second_asset, double second_value,
                 boost::intrusive_ptr<
                     aos::impl::SlidingWindowStorageObservable<size_t, double>>
                     value) {
    // Ждем своей очереди
    while (current_index.load(std::memory_order_acquire) != index) {
        std::this_thread::yield();  // Отдаем управление другим потокам
    }

    // Передаем данные строго в порядке индекса
    std::cout << "Processing: " << first_value << " " << second_value
              << std::endl;
    value->AddData(hash_first_asset, first_value);
    value->AddData(hash_second_asset, second_value);

    // Обновляем индекс, чтобы следующий поток мог продолжить
    current_index.fetch_add(1, std::memory_order_release);
}

int main() {
    boost::asio::thread_pool thread_pool;
    aos::impl::SlidingWindowStorageFactory factory(thread_pool);
    auto [sliding_window_storage, mi_calculator, deviation_tracker,
          market_triplet_manager] = factory.Create();

    market_triplet_manager->Connect(1, 2);

    aot::CoBus bus(thread_pool);
    Trading::OrderBookComponent order_book_spot_component(
        boost::asio::make_strand(thread_pool), bus, 1000,
        common::MarketType::kSpot);
    common::TradingPair trading_pair{2, 1};
    order_book_spot_component.AddOrderBook(common::ExchangeId::kBinance,
                                           trading_pair);

    Exchange::MEMarketUpdate2Pool me_market_update_pool(123);
    auto ptr = me_market_update_pool.Allocate(
        &me_market_update_pool, common::ExchangeId::kBinance,
        common::TradingPair{2, 1}, Exchange::MarketUpdateType::DEFAULT, 1,
        common::Side::kAsk, 10000, 123);
    Exchange::BusEventMEMarketUpdate2Pool pool_bus(123);
    auto bus_ptr = pool_bus.Allocate(
        &pool_bus, boost::intrusive_ptr<Exchange::MEMarketUpdate2>(ptr));

    order_book_spot_component.AsyncHandleEvent(
        boost::intrusive_ptr<Exchange::BusEventMEMarketUpdate2>(bus_ptr));

    sliding_window_storage->AddActionsToBuy(
        [&sliding_window_storage](std::queue<size_t>& queue, const size_t hash,
                                  const double& value) {
            if (hash == 1) {
                logi("start analise hash:{} value:{}", hash, value);
                queue.push(hash);
            }
        });

    sliding_window_storage->AddActionsToBuy(
        [&sliding_window_storage](std::queue<size_t>& queue, const size_t hash,
                                  const double& value) {
            auto [status_deviation_ratio, deviation_ratio] =
                sliding_window_storage->GetDeviationRatio(hash, value);
            auto deviation = sliding_window_storage->GetDeviation(hash, value);
            logi(
                "Before adding hash:{} value:{} deviation_ratio:{} "
                "deviation:{}",
                hash, value, deviation_ratio, deviation);
            if (deviation_ratio > 0.001) {
                queue.push(hash);
            }
        });

    sliding_window_storage->AddActionsToBuy(
        [&mi_calculator, &sliding_window_storage, &market_triplet_manager](
            std::queue<size_t>& queue, const size_t hash_asset,
            const double& value) {
            if (!market_triplet_manager->HasPair(hash_asset)) return;
            for (const auto& paired_asset :
                 market_triplet_manager->GetPairs(hash_asset)) {
                auto [status, mi] = mi_calculator->ComputeMutualInformation(
                    sliding_window_storage, hash_asset, paired_asset, 10);
                if (!status) continue;
                logi("Mutual Information ({} ↔ {}):{}", hash_asset,
                     paired_asset, mi);
                if (mi > 2) {
                    queue.push(paired_asset);
                }
            }
        });

    auto strand = boost::asio::make_strand(thread_pool);
    sliding_window_storage->AddActionsToBuy(
        [&strand, &order_book_spot_component](std::queue<size_t>& queue,
                                              const size_t hash_asset,
                                              const double& value) {
            logi("Need buy hash:{} value:{}", hash_asset, value);

            boost::asio::co_spawn(
                strand,
                [exchange_id  = common::ExchangeId::kBinance,
                 trading_pair = common::TradingPair{2, 1},
                 &order_book_spot_component,
                 level = 0]() -> boost::asio::awaitable<void> {
                    auto [price, qty] =
                        co_await order_book_spot_component
                            .AsyncGetPriceAndQtyAtLevelAsk(exchange_id,
                                                           trading_pair, level);

                    logi("Need buy {} Fetched Price: {}, Qty: {}", trading_pair,
                         price, qty);
                    co_return;
                },
                boost::asio::detached);  // Запускаем без ожидания завершения

            logi("Корутина запущена, продолжаем выполнение кода.");
        });

    sliding_window_storage->AddActionsToSell(
        [&sliding_window_storage](std::queue<size_t>& queue, const size_t hash,
                                  const double& value) {
            if (hash == 1) {
                logi("start analise hash:{} value:{}", hash, value);
                queue.push(hash);
            }
        });

    sliding_window_storage->AddActionsToSell(
        [&sliding_window_storage](std::queue<size_t>& queue, const size_t hash,
                                  const double& value) {
            auto [status_deviation_ratio, deviation_ratio] =
                sliding_window_storage->GetDeviationRatio(hash, value);
            auto deviation = sliding_window_storage->GetDeviation(hash, value);
            logi(
                "Before adding hash:{} value:{} deviation_ratio:{} "
                "deviation:{}",
                hash, value, deviation_ratio, deviation);
            if (deviation_ratio < 0.001) {
                queue.push(hash);
            }
        });

    sliding_window_storage->AddActionsToSell(
        [&mi_calculator, &sliding_window_storage, &market_triplet_manager](
            std::queue<size_t>& queue, const size_t hash_asset,
            const double& value) {
            if (!market_triplet_manager->HasPair(hash_asset)) return;
            for (const auto& paired_asset :
                 market_triplet_manager->GetPairs(hash_asset)) {
                auto [status, mi] = mi_calculator->ComputeMutualInformation(
                    sliding_window_storage, hash_asset, paired_asset, 10);
                if (!status) continue;
                logi("Mutual Information ({} ↔ {}):{}", hash_asset,
                     paired_asset, mi);
                if (mi > 2) {
                    queue.push(paired_asset);
                }
            }
        });

    sliding_window_storage->AddActionsToSell([](std::queue<size_t>& queue,
                                                const size_t hash_asset,
                                                const double& value) {
        logi("Need sell hash:{} value:{}", hash_asset, value);
        return true;
    });

    // sliding_window_storage->AddObserverAfterAdd(
    //     [](std::queue<size_t>& queue, const size_t hash, const double& value)
    //     {
    //         logi("After adding hash:{} value:{}", hash, value);
    //         return true;
    //     });
    std::vector<std::pair<double, double>> incoming_data = {
        {29300.25, 29290.10}, {29350.50, 29340.35}, {29400.75, 29380.20},
        {29450.00, 29430.15}, {29500.25, 29488.05}, {29550.50, 29520.40},
        {29602.75, 29560.60}, {29000.00, 29600.50}, {49700.25, 29650.40},
        {29750.50, 59700.35}};
    size_t hash_first_asset  = 1;
    size_t hash_second_asset = 2;

    std::vector<std::thread> threads;
    for (size_t i = 0; i < incoming_data.size(); ++i) {
        threads.emplace_back(ProcessData, i, hash_first_asset,
                             incoming_data[i].first, hash_second_asset,
                             incoming_data[i].second, sliding_window_storage);
    }

    // for (const auto& data : incoming_data) {
    //     // threads.emplace_back([&sliding_window_storage, hash_first_asset,
    //     //                       hash_second_asset, data]() {
    //     sliding_window_storage->AddData(hash_first_asset, data.first);
    //     sliding_window_storage->AddData(hash_second_asset, data.second);
    //     //});
    // }

    // // Ожидаем завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }
    sliding_window_storage->Wait();

    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    boost::asio::co_spawn(
        strand,
        [&promise]() -> boost::asio::awaitable<void> {
            co_await boost::asio::this_coro::executor;  // Ждем выполнения всех
                                                        // задач в strand
            promise.set_value();  // Сообщаем, что все завершилось
            co_return;
        },
        boost::asio::detached);
    future.get();
    // for (const auto& data : incoming_data) {
    //     sliding_window_storage->AddData(hash_first_asset, data.first);
    //     sliding_window_storage->AddData(hash_second_asset, data.second);
    //     // i++;
    // }
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    logi("finish");
    fmtlog::poll();
    // Убираем указатели
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    // mi_calculator                 = nullptr;
    // hist_calculator               = nullptr;
    // joint_hist_calculator         = nullptr;
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
