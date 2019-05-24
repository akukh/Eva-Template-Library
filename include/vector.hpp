#pragma once
#include "algorithm.hpp"
#include "allocator.hpp"

namespace etl {

template <typename T, typename A = allocator<T>>
class vector {
public:
    typedef T*             pointer;
    typedef pointer        iterator;
    typedef iterator const const_iterator;
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

    void clear();

    void push_back(T const& value);

private:
    void destroy_elements() noexcept;

    template <typename Type, typename Alloc = allocator<T>>
    struct vector_base;

    vector_base<T, A> base_;
};

} // namespace etl

// Implementation:
namespace etl {

template <typename T, typename A>
template <typename Type, typename Alloc>
struct vector<T, A>::vector_base {
    typedef Alloc       allocator_type;
    typedef std::size_t size_type;
    typedef Type*       pointer;

    allocator_type allocator_;
    pointer        element_;
    pointer        space_;
    pointer        last_;

    vector_base(allocator_type const& allocator, typename allocator_type::size_type const n)
        : allocator_{allocator}, element_{allocator_.allocate(n)}, space_{element_ + n}, last_{element_ + n} {}
    ~vector_base() { allocator_.deallocate(element_, last_ - element_); }

    vector_base(vector_base const& other) = delete;
    vector_base& operator=(vector_base const& other) = delete;

    vector_base(vector_base&& other)
        : allocator_{other.allocator_}, element_{other.element_}, space_{other.space_}, last_{other.last_} {
        element_ = space_ = last_;
    }
    vector_base& operator=(vector_base&& other) {
        std::swap(*this, other);
        return *this;
    }
};

template <typename T, typename A>
vector<T, A>::vector(std::size_t const n, T const& value, A const& allocator) : base_{allocator, n} {
    T* current;
    try {
        T* last = base_.element_ + n;
        for (current = base_.element_; current != last; ++current) {
            base_.allocator_.construct(current, value);
        }
    } catch (...) {
        for (T* it = base_.element_; it != current; ++it) {
            base_.allocator_.destroy(it);
        }
        throw;
    }
}

template <typename T, typename A>
vector<T, A>::vector(vector const& other) : base_{other.base_.allocator_, other.size()} {
    // uninitialized_copy(base_.element);
}

template <typename T, typename A>
vector<T, A>& vector<T, A>::operator=(vector const& other) {
    /*
    if (capacity() < other.size()) {
        vector temp{other};
        std::swap(*this, temp);
        return *this;
    }

    if (&other == *this) {
        return *this;
    }

    std::size_t size       = size();
    std::size_t other_size = other.size();
    allocator_             = other.allocator_;

    if (other_size <= size) {
        for (T* it = other.element_; it != other.element_ + size;) {
            *element_++ = *it++;
        }
        for (T* it = element_ + size; it != space_; ++it) {
            allocator_.destroy(it);
        }
    }
    */
}

template <typename T, typename A>
vector<T, A>::vector(vector&& other) {}

template <typename T, typename A>
vector<T, A>& vector<T, A>::operator=(vector&& other) {
    std::swap(*this, other);
    return *this;
}

template <typename T, typename A>
vector<T, A>::~vector() {
    destroy_elements();
}

template <typename T, typename A>
void vector<T, A>::destroy_elements() noexcept {
    for (T* it = base_.element_; it != base_.space_; ++it) {
        base_.allocator_.destroy(it);
    }
    base_.space_ = base_.element_;
}

template <typename T, typename A>
std::size_t vector<T, A>::size() const noexcept {
    return base_.space_ - base_.element_;
}

template <typename T, typename A>
std::size_t vector<T, A>::capacity() const noexcept {
    return base_.last_ - base_.element_;
}

} // namespace etl