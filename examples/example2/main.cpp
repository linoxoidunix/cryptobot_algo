#include <functional>
#include <iostream>
#include <vector>

class DataReceiver {
  public:
    // Добавление наблюдателей перед вставкой данных
    void AddBeforeObserver(std::function<void(int)> observer) {
        beforeObservers_.push_back(std::move(observer));
    }

    // Добавление наблюдателей после вставки данных
    void AddAfterObserver(std::function<void()> observer) {
        afterObservers_.push_back(std::move(observer));
    }

    void AddData(int value) {
        // Вызываем наблюдателей ДО добавления данных
        for (const auto& observer : beforeObservers_) {
            observer(value);
        }

        data_.push_back(value);

        // Вызываем наблюдателей ПОСЛЕ добавления данных
        for (const auto& observer : afterObservers_) {
            observer();
        }
    }

    const std::vector<int>& GetData() const { return data_; }

  private:
    std::vector<int> data_;
    std::vector<std::function<void(int)>> beforeObservers_;
    std::vector<std::function<void()>> afterObservers_;
};

class DataProcessor {
  public:
    explicit DataProcessor(DataReceiver& receiver) : receiver_(receiver) {
        receiver_.AddAfterObserver([this]() { SumData(); });
    }

    void SumData() {
        int sum = 0;
        for (int value : receiver_.GetData()) {
            sum += value;
        }
        std::cout << "Sum: " << sum << std::endl;
    }

  private:
    DataReceiver& receiver_;
};

class Logger {
  public:
    explicit Logger(DataReceiver& receiver) {
        receiver.AddBeforeObserver([this](int value) { LogBeforeAdd(value); });
        receiver.AddAfterObserver([this]() { LogAfterAdd(); });
    }

    static void LogBeforeAdd(int value) {
        std::cout << "[Logger] Будет добавлено: " << value << std::endl;
    }

    static void LogAfterAdd() {
        std::cout << "[Logger] Данные добавлены!" << std::endl;
    }
};

int main() {
    DataReceiver receiver;
    DataProcessor processor(receiver);
    Logger logger(receiver);  // Логирование операций

    receiver.AddData(10);
    receiver.AddData(20);
    receiver.AddData(30);

    return 0;
}
