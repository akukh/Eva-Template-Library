#pragma once
#include "allocator.hpp"

namespace etl {

template <typename T, typename A = allocator<T>>
class vector {
public:
    explicit vector(std::size_t const n, T const& value = T(), A const& allocator = A());

    vector(vector const& other);
    vector& operator=(vector const& other);

    vector(vector&& other);
    vector& operator=(vector&& other);

    ~vector();

    std::size_t size() const noexcept;
    std::size_t capacity() const noexcept;

    void reserve(std::size_t const n);
    void resize(std::size_t const n, T const& value = T());

    void push_back(T const& value);

private:
    T* element_;
    T* space_;
    T* last_;
    A  allocator_;
};

} // namespace etl

// Implementation:
namespace etl {

template <typename T, typename A>
vector<T, A>::vector(std::size_t const n, T const& value, A const& allocator) : allocator_{allocator} {
    element_ = allocator_.allocate(n);
    T* p;

    try {
        T* last = element_ + n;
        for (p = element_; p != last; ++p) {
            allocator_.construct(p, value);
        }
        last_ = space_ = p;
    } catch (...) {
        for (T* it = element_; it != p; ++it) {
            allocator_.destroy(it);
        }
        allocator_.deallocate(element_, n);
        throw;
    }
}

template <typename T, typename A>
vector<T, A>::vector(vector const& other) {
    allocator_.allocate(other.size());

    T* other_first = other.element_;
    T* other_last  = other.last_;
    T* current     = element_;
    try {
        for (; other_first != other_last; ++other_first, ++current) {
            allocator_.construct(current, *other_first);
        }
        return current;
    } catch (...) {
        for (T* it = element_; it != current; ++it) {
            allocator_.destroy(it);
        }
        allocator_.deallocate(element_, other.size());
        throw;
    }
}

template <typename T, typename A>
std::size_t vector<T, A>::size() const noexcept {
    return space_ - element_;
}

template <typename T, typename A>
std::size_t vector<T, A>::capacity() const noexcept {
    return last_ - element_;
}

} // namespace etl