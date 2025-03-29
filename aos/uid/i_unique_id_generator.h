#pragma once
namespace aos {
// Интерфейс для генератора уникальных идентификаторов
template <typename T, template <typename> class MemoryPool>
class IUniqueIDGenerator
    : public common::RefCounted<MemoryPool, IUniqueIDGenerator<T, MemoryPool>> {
  public:
    virtual ~IUniqueIDGenerator() = default;
    virtual T GenerateUniqueID()  = 0;
};
};  // namespace aos