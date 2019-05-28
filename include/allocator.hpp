#pragma once
#include <cstddef>
#include <cstdint>

// TODO:
//  get rid of the unnecessary headers
#include <utility>
#include <limits>

namespace etl {

template <typename T>
struct allocator {
    typedef T              value_type;
    typedef value_type*    pointer;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;

    [[nodiscard]] pointer allocate(size_type const n);
    void                  deallocate(pointer p, [[maybe_unused]] size_type const n) noexcept;

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args);

    template <typename U>
    void destroy(U* p) noexcept;

    size_type max_size() const noexcept;

    template <typename U>
    struct rebind { typedef allocator<U> other; };
};

} // namespace etl

// Implementation:
namespace etl {

template <typename T>
[[nodiscard]] typename allocator<T>::pointer allocator<T>::allocate(size_type const n) {
    if (n > max_size()) {
        throw "bad_alloc"; // TODO: throw bad_alloc()
    }
    return static_cast<pointer>(::operator new(n * sizeof(T)));
}

template <typename T>
void allocator<T>::deallocate(pointer p, [[maybe_unused]] size_type const n) noexcept {
    ::operator delete(p);
}

template <typename T>
template <typename U, typename... Args>
void allocator<T>::construct(U* p, Args&&... args) {
    ::new(static_cast<void*>(&*p)) U(std::forward<Args>(args)...);
}

template <typename T>
template <typename U>
void allocator<T>::destroy(U* p) noexcept {
    (&*p)->~U();
}

template <typename T>
typename allocator<T>::size_type allocator<T>::max_size() const noexcept {
    return std::numeric_limits<T>::max() / sizeof(T);
}

} // namespace etl
