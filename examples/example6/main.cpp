#include <iostream>
#include <map>
#include <vector>

struct Order {
    double price;
    int volume;
};

class OrderBook {
  public:
    // Мапа для хранения цен по уровням
    std::map<int, Order>
        priceLevelMap;  // ключ: уровень, значение: Order (цена и количество)

    // Мапа для хранения уровней по ценам
    std::map<double, int> priceToLevelMap;  // ключ: цена, значение: уровень

    // Добавление ордера в книгу ордеров
    void addOrder(double price, int volume) {
        // Определяем уровень по количеству существующих ордеров
        int level              = priceLevelMap.size();

        // Добавляем ордер в книгу ордеров
        priceLevelMap[level]   = {price, volume};
        // Обновляем priceToLevelMap, связывая цену с уровнем
        priceToLevelMap[price] = level;
    }

    // Удаление ордера по цене
    void removeOrder(double price) {
        if (priceToLevelMap.find(price) != priceToLevelMap.end()) {
            int level = priceToLevelMap[price];
            priceLevelMap.erase(level);
            priceToLevelMap.erase(price);
        }
    }

    // Обновление ордера по цене
    void updateOrder(double oldPrice, double newPrice, int newVolume) {
        if (priceToLevelMap.find(oldPrice) != priceToLevelMap.end()) {
            int level            = priceToLevelMap[oldPrice];
            priceLevelMap[level] = {newPrice, newVolume};

            // Обновляем priceToLevelMap
            priceToLevelMap.erase(oldPrice);    // Удаляем старую цену
            priceToLevelMap[newPrice] = level;  // Добавляем новую цену
        }
    }

    // Вывод книги ордеров
    void printOrderBook() {
        std::cout << "Order Book:\n";
        for (const auto& entry : priceLevelMap) {
            std::cout << "Level: " << entry.first
                      << " -> Price: " << entry.second.price
                      << ", Volume: " << entry.second.volume << "\n";
        }
        std::cout << "\nPrice to Level Map:\n";
        for (const auto& entry : priceToLevelMap) {
            std::cout << "Price: " << entry.first
                      << " -> Level: " << entry.second << "\n";
        }
    }
};

int main() {
    OrderBook book;

    // Добавление ордеров в книгу
    book.addOrder(100.5, 50);  // Цена 100.5, Количество 50
    book.addOrder(101.0, 60);  // Цена 101.0, Количество 60
    book.addOrder(102.5, 30);  // Цена 102.5, Количество 30
    book.addOrder(91.5, 30);   // Цена 102.5, Количество 30
    book.addOrder(85.5, 30);   // Цена 102.5, Количество 30
    book.addOrder(86.5, 30);   // Цена 102.5, Количество 30

    // Обновление ордера
    book.updateOrder(101.0, 101.2,
                     55);  // Изменение цены с 101.0 на 101.2 и количества на 55

    // Удаление ордера
    book.removeOrder(102.5);

    // Печать книги ордеров
    book.printOrderBook();

    return 0;
}