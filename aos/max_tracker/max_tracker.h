#pragma once
#include <map>

#include "aos/logger/mylog.h"
#include "aos/max_tracker/i_max_tracker.h"

namespace aos {
namespace impl {

template <typename HashT, typename T>
class MaxTrackerDefault : public MaxTrackerInterface<HashT, T> {
  public:
    void OnAdd(const HashT& hash_asset, const T& value) override {
        if (max_element_.find(hash_asset) == max_element_.end()) {
            // Если еще нет записей для данного ключа, инициализируем
            frequency_[hash_asset]   = std::map<T, size_t>();
            max_element_[hash_asset] = value;
        }

        // Увеличиваем количество элемента в карте
        frequency_[hash_asset][value]++;

        // Обновляем максимальный элемент
        if (max_element_[hash_asset] < value) {
            max_element_[hash_asset] = value;
        }

        logi("Add {} for {}, new max: {}", value, hash_asset,
             max_element_[hash_asset]);
    }

    void OnRemove(const HashT& hash_asset, const T& value) override {
        auto it = frequency_.find(hash_asset);
        if (it == frequency_.end()) {
            return;  // Нет данных для данного хэш-актива
        }

        auto& asset_map = it->second;
        auto value_it   = asset_map.find(value);
        if (value_it == asset_map.end()) {
            return;  // Элемент для удаления не найден
        }

        // Уменьшаем количество элемента
        if (--(value_it->second) == 0) {
            asset_map.erase(
                value_it);  // Удаляем элемент, если его количество стало 0
        }

        // Если удаляется максимальный элемент, пересчитываем максимум
        if (max_element_[hash_asset] == value) {
            if (asset_map.empty()) {
                // Если больше нет элементов для данного актива
                max_element_.erase(hash_asset);
                logi("Remove {} for {}, no more elements, max not found", value,
                     hash_asset);
            } else {
                // Пересчитываем максимальный элемент
                max_element_[hash_asset] = asset_map.rbegin()->first;
                logi("Remove {} for {}, new max: {}", value, hash_asset,
                     max_element_[hash_asset]);
            }
        } else {
            logi("Remove {} for {}, no change in max: {}", value, hash_asset,
                 max_element_[hash_asset]);
        }
    }

    std::pair<bool, T> GetMax(const HashT& hash_asset) const override {
        auto it = max_element_.find(hash_asset);
        if (it == max_element_.end()) {
            return {false, T{}};  // Если максимальный элемент не найден
        }
        return {true, it->second};  // Возвращаем максимальный элемент
    }

    ~MaxTrackerDefault() override { logi("MaxTracker dtor"); }

  private:
    std::unordered_map<HashT, std::map<T, size_t>>
        frequency_;  // Карта для отслеживания частоты элементов
    std::unordered_map<HashT, T>
        max_element_;  // Карта для отслеживания максимальных элементов
};

template <typename HashT, typename T,
          template <typename> typename MemoryPoolNotThreadSafety>
class MaxTracker
    : public aos::IMaxTracker<HashT, T, MemoryPoolNotThreadSafety> {
  public:
    void OnAdd(const HashT& hash_asset, const T& value) override {
        if (max_element_.find(hash_asset) == max_element_.end()) {
            // Если еще нет записей для данного ключа, инициализируем
            frequency_[hash_asset]   = std::map<T, size_t>();
            max_element_[hash_asset] = value;
        }

        // Увеличиваем количество элемента в карте
        frequency_[hash_asset][value]++;

        // Обновляем максимальный элемент
        if (max_element_[hash_asset] < value) {
            max_element_[hash_asset] = value;
        }

        logi("Add {} for {}, new max: {}", value, hash_asset,
             max_element_[hash_asset]);
    }

    void OnRemove(const HashT& hash_asset, const T& value) override {
        auto it = frequency_.find(hash_asset);
        if (it == frequency_.end()) {
            return;  // Нет данных для данного хэш-актива
        }

        auto& asset_map = it->second;
        auto value_it   = asset_map.find(value);
        if (value_it == asset_map.end()) {
            return;  // Элемент для удаления не найден
        }

        // Уменьшаем количество элемента
        if (--(value_it->second) == 0) {
            asset_map.erase(
                value_it);  // Удаляем элемент, если его количество стало 0
        }

        // Если удаляется максимальный элемент, пересчитываем максимум
        if (max_element_[hash_asset] == value) {
            if (asset_map.empty()) {
                // Если больше нет элементов для данного актива
                max_element_.erase(hash_asset);
                logi("Remove {} for {}, no more elements, max not found", value,
                     hash_asset);
            } else {
                // Пересчитываем максимальный элемент
                max_element_[hash_asset] = asset_map.rbegin()->first;
                logi("Remove {} for {}, new max: {}", value, hash_asset,
                     max_element_[hash_asset]);
            }
        } else {
            logi("Remove {} for {}, no change in max: {}", value, hash_asset,
                 max_element_[hash_asset]);
        }
    }

    std::pair<bool, T> GetMax(const HashT& hash_asset) const override {
        auto it = max_element_.find(hash_asset);
        if (it == max_element_.end()) {
            return {false, T{}};  // Если максимальный элемент не найден
        }
        return {true, it->second};  // Возвращаем максимальный элемент
    }

    ~MaxTracker() override { logi("MaxTracker dtor"); }

  private:
    std::unordered_map<HashT, std::map<T, size_t>>
        frequency_;  // Карта для отслеживания частоты элементов
    std::unordered_map<HashT, T>
        max_element_;  // Карта для отслеживания максимальных элементов
};

template <typename HashT, typename T,
          template <typename> typename MemoryPoolNotThreadSafety>
class MaxTrackerBuilder {
  public:
    explicit MaxTrackerBuilder(
        MemoryPoolNotThreadSafety<
            MaxTracker<HashT, T, MemoryPoolNotThreadSafety>>& pool)
        : pool_(pool) {}

    // Строим объект MinTracker с заданным пулом памяти
    boost::intrusive_ptr<MaxTracker<HashT, T, MemoryPoolNotThreadSafety>>
    build() {
        auto* obj = pool_.Allocate();
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<
            MaxTracker<HashT, T, MemoryPoolNotThreadSafety>>(obj);
    }

  private:
    MemoryPoolNotThreadSafety<MaxTracker<HashT, T, MemoryPoolNotThreadSafety>>&
        pool_;  // Пул памяти
};
};  // namespace impl
};  // namespace aos