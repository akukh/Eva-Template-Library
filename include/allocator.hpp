#pragma once
#include <cstddef>
#include <cstdint>

namespace etl {

template <typename T>
struct allocator {
    [[nodiscard]] T* allocate(std::size_t const n);
    void             deallocate(T* p, std::size_t const n);

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args);

    std::size_t max_size() const noexcept;
};

} // namespace etl

// Implementation:
namespace etl {

template <typename T>
[[nodiscard]] T* allocator<T>::allocate(std::size_t const n) { return static_cast<T*>(::operator new(n * sizeof(T))); }

template <typename T>
void allocator<T>::deallocate(T* p, std::size_t const n) {
    ::operator delete(p);
}

template <typename T>
template <typename U, typename... Args>
void allocator<T>::construct(U* p, Args&&... args) {
    ::new (p) U(std::forward<Args>(args)...);
}

template <typename T>
std::size_t allocator<T>::max_size() const noexcept {
    return std::numeric_limits<T>::max() / sizeof(T);
}

} // namespace etl