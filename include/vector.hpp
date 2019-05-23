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
    void resize(std::size_t const n, T const& value = ());

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
vector<T, A>::vector(std::size_t const n, T const& value = T(), A const& allocator = A()) : allocator_{allocator} {
    element_ = allocator_.allocate(n);
    T* p;

    try {
        T* end = element_ + n;
        for (p = element_; p != end; ++p) {
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
vector<T, A>::vector(vector const& other) {}

template <typename T, typename A>
std::size_t vector<T, A>::size() const noexcept {
    return space_ - element_;
}

template <typename T, typename A>
std::size_t vector<T, A>::capacity() const noexcept {
    return last_ - element_;
}

} // namespace etl