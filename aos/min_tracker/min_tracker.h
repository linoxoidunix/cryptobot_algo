#pragma once
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <map>
#include <unordered_map>

#include "aos/logger/mylog.h"
#include "aos/min_tracker/i_min_tracker.h"

namespace aos {
namespace impl {

template <typename HashT, typename T>
class MinTrackerDefault : public MinTrackerInterface<HashT, T> {
  public:
    void OnAdd(const HashT& hash_asset, const T& value) override {
        if (min_element_.find(hash_asset) == min_element_.end()) {
            // Если еще нет записей для данного ключа, инициализируем
            frequency_[hash_asset]   = std::map<T, size_t>();
            min_element_[hash_asset] = value;
        }

        // Увеличиваем количество элемента в карте
        frequency_[hash_asset][value]++;

        // Обновляем минимальный элемент
        if (min_element_[hash_asset] > value) {
            min_element_[hash_asset] = value;
        }

        logi("Add {} for {}, new min: {}", value, hash_asset,
             min_element_[hash_asset]);
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

        // Если удаляется минимальный элемент, пересчитываем минимум
        if (min_element_[hash_asset] == value) {
            if (asset_map.empty()) {
                // Если больше нет элементов для данного актива
                min_element_.erase(hash_asset);
                logi("Remove {} for {}, no more elements, min not found", value,
                     hash_asset);
            } else {
                // Пересчитываем минимальный элемент
                min_element_[hash_asset] = asset_map.begin()->first;
                logi("Remove {} for {}, new min: {}", value, hash_asset,
                     min_element_[hash_asset]);
            }
        } else {
            logi("Remove {} for {}, no change in min: {}", value, hash_asset,
                 min_element_[hash_asset]);
        }
    }

    std::pair<bool, T> GetMin(const HashT& hash_asset) const override {
        auto it = min_element_.find(hash_asset);
        if (it == min_element_.end()) {
            return {false, T{}};  // Если минимальный элемент не найден
        }
        return {true, it->second};  // Возвращаем минимальный элемент
    }

    ~MinTrackerDefault() override { logi("MinTrackerDefault dtor"); }

  private:
    std::unordered_map<HashT, std::map<T, size_t>>
        frequency_;  // Карта для отслеживания частоты элементов
    std::unordered_map<HashT, T>
        min_element_;  // Карта для отслеживания минимальных элементов
};

template <typename HashT, typename T,
          template <typename> typename MemoryPoolNotThreadSafety>
class MinTracker
    : public aos::IMinTracker<HashT, T, MemoryPoolNotThreadSafety> {
  public:
    void OnAdd(const HashT& hash_asset, const T& value) override {
        if (min_element_.find(hash_asset) == min_element_.end()) {
            // Если еще нет записей для данного ключа, инициализируем
            frequency_[hash_asset]   = std::map<T, size_t>();
            min_element_[hash_asset] = value;
        }

        // Увеличиваем количество элемента в карте
        frequency_[hash_asset][value]++;

        // Обновляем минимальный элемент
        if (min_element_[hash_asset] > value) {
            min_element_[hash_asset] = value;
        }

        logi("Add {} for {}, new min: {}", value, hash_asset,
             min_element_[hash_asset]);
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

        // Если удаляется минимальный элемент, пересчитываем минимум
        if (min_element_[hash_asset] == value) {
            if (asset_map.empty()) {
                // Если больше нет элементов для данного актива
                min_element_.erase(hash_asset);
                logi("Remove {} for {}, no more elements, min not found", value,
                     hash_asset);
            } else {
                // Пересчитываем минимальный элемент
                min_element_[hash_asset] = asset_map.begin()->first;
                logi("Remove {} for {}, new min: {}", value, hash_asset,
                     min_element_[hash_asset]);
            }
        } else {
            logi("Remove {} for {}, no change in min: {}", value, hash_asset,
                 min_element_[hash_asset]);
        }
    }

    std::pair<bool, T> GetMin(const HashT& hash_asset) const override {
        auto it = min_element_.find(hash_asset);
        if (it == min_element_.end()) {
            return {false, T{}};  // Если минимальный элемент не найден
        }
        return {true, it->second};  // Возвращаем минимальный элемент
    }

    ~MinTracker() override { logi("MinTracker dtor"); }

  private:
    std::unordered_map<HashT, std::map<T, size_t>>
        frequency_;  // Карта для отслеживания частоты элементов
    std::unordered_map<HashT, T>
        min_element_;  // Карта для отслеживания минимальных элементов
};

template <typename HashT, typename T,
          template <typename> typename MemoryPoolNotThreadSafety>
class MinTrackerBuilder {
  public:
    explicit MinTrackerBuilder(
        MemoryPoolNotThreadSafety<
            MinTracker<HashT, T, MemoryPoolNotThreadSafety>>& pool)
        : pool_(pool) {}

    // Строим объект MinTracker с заданным пулом памяти
    boost::intrusive_ptr<MinTracker<HashT, T, MemoryPoolNotThreadSafety>>
    build() {
        auto* obj = pool_.Allocate();
        obj->SetMemoryPool(&pool_);
        return boost::intrusive_ptr<
            MinTracker<HashT, T, MemoryPoolNotThreadSafety>>(obj);
    }

  private:
    MemoryPoolNotThreadSafety<MinTracker<HashT, T, MemoryPoolNotThreadSafety>>&
        pool_;  // Пул памяти
};
};  // namespace impl
};  // namespace aos