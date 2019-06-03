#pragma once
#include "algorithm.hpp"
#include "allocator.hpp"
#include "iterator.hpp"
#include "type_traits.hpp"

namespace etl {

template <typename Type, typename Allocator>
struct vector_base {
    typedef Type                                   value_type;
    typedef Allocator                              allocator_type;
    typedef allocator_traits<allocator_type>       alloc_traits;
    typedef value_type&                            reference;
    typedef value_type const&                      const_reference;
    typedef typename alloc_traits::size_type       size_type;
    typedef typename alloc_traits::difference_type difference_type;
    typedef typename alloc_traits::pointer         pointer;
    typedef typename alloc_traits::const_pointer   const_pointer;
    typedef pointer                                iterator;
    typedef const_pointer                          const_iterator;

    allocator_type allocator_;
    pointer        begin_;
    pointer        capacity_;
    pointer        end_;

    vector_base(allocator_type const& allocator, typename allocator_type::size_type const n);
    ~vector_base();

    vector_base(vector_base const& other) = delete;
    vector_base& operator=(vector_base const& other) = delete;

    vector_base(vector_base&& other) noexcept;
    vector_base& operator=(vector_base&& other) noexcept;

    void clear() noexcept;

    void destruct_at_end(pointer new_last) noexcept;
    void copy_assign_alloc(vector_base const& other);
};

template <typename T, typename A = allocator<T>>
class vector : private vector_base<T, A> {
    typedef vector_base<T, A> base;

public:
    typedef T                              value_type;
    typedef A                              allocator_type;
    typedef typename base::alloc_traits    alloc_traits;
    typedef typename base::size_type       size_type;
    typedef typename base::difference_type difference_type;
    typedef typename base::reference       reference;
    typedef typename base::const_reference const_reference;
    typedef typename base::pointer         pointer;
    typedef typename base::const_pointer   const_pointer;
    typedef wrap_iter<pointer>             iterator;
    typedef wrap_iter<const_pointer>       const_iterator;
    // TODO:
    //  implement reverse iterators

    explicit vector(allocator_type const& allocator = A()) noexcept;
    explicit vector(size_type const n, value_type const& value = T(), allocator_type const& allocator = A());

    vector(vector const& other);
    vector& operator=(vector const& other);

    vector(vector&& other) noexcept;
    vector& operator=(vector&& other) noexcept;

    ~vector();

    iterator       begin() noexcept;
    const_iterator begin() const noexcept;

    iterator       end() noexcept;
    const_iterator end() const noexcept;

    size_type size() const noexcept;
    size_type capacity() const noexcept;

    bool empty() const noexcept;

    void reserve(size_type const n);
    void resize(size_type const n, value_type const& value = T());

    void clear() noexcept;

    void push_back(value_type const& value);

    reference       operator[](size_type const n);
    const_reference operator[](size_type const n) const;

private:
    using base::allocator_;
    using base::begin_;
    using base::capacity_;
    using base::end_;

    template <typename InputIterator>
    void assign(InputIterator first, InputIterator last);

    void destroy_elements() noexcept;

    iterator       capacity_end() noexcept;
    const_iterator capacity_end() const noexcept;
};

} // namespace etl

// Implementation:
namespace etl {

template <typename Type, typename Allocator>
vector_base<Type, Allocator>::vector_base(allocator_type const& allocator, typename allocator_type::size_type const n)
    : allocator_{allocator}, begin_{allocator_.allocate(n)}, capacity_{begin_ + n}, end_{begin_ + n} {}

template <typename Type, typename Allocator>
vector_base<Type, Allocator>::~vector_base() {
    allocator_.deallocate(begin_, end_ - begin_);
}

template <typename Type, typename Allocator>
vector_base<Type, Allocator>::vector_base(vector_base&& other) noexcept
    : allocator_{other.allocator_}, begin_{other.begin_}, capacity_{other.capacity_}, end_{other.end_} {
    begin_ = end_ = capacity_ = nullptr;
}

template <typename Type, typename Allocator>
vector_base<Type, Allocator>& vector_base<Type, Allocator>::operator=(vector_base&& other) noexcept {
    std::swap(*this, other);
    return *this;
}

template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::clear() noexcept {
    destruct_at_end(begin_);
}

template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::destruct_at_end(pointer new_last) noexcept {
    pointer soon_to_be_end = end_;
    while (new_last != soon_to_be_end) {
        alloc_traits::destroy(allocator_, --soon_to_be_end);
    }
    end_ = new_last;
}

template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::copy_assign_alloc(vector_base const& other) {
    if (allocator_ != other.allocator_) {
        clear();
        alloc_traits::deallocate(allocator_, begin_, capacity_ - begin_);
        begin_ = end_ = capacity_ = nullptr;
    }
    allocator_ = other.allocator_;
}

template <typename T, typename A>
vector<T, A>::vector(allocator_type const& allocator) noexcept : base(allocator, 4) {}

template <typename T, typename A>
vector<T, A>::vector(size_type const n, value_type const& value, allocator_type const& allocator) : base(allocator, n) {
    pointer current;
    try {
        pointer end = end_;
        for (current = begin_; current != end; ++current) {
            allocator_.construct(current, value);
        }
    } catch (...) {
        for (pointer it = begin_; it != current; ++it) {
            allocator_.destroy(it);
        }
        throw;
    }
}

template <typename T, typename A>
vector<T, A>::vector(vector const& other) : base(other.allocator_, other.size()) {
    pointer first   = other.begin_;
    pointer last    = other.end_;
    pointer current = begin_;

    try {
        for (; first != last; ++first, ++current) {
            allocator_.construct(current, *last);
        }
    } catch (...) {
        for (pointer it = begin_; it != current; ++it) {
            allocator_.destroy(it);
        }
        throw;
    }
}

template <typename T, typename A>
template <typename InputIterator>
void vector<T, A>::assign(InputIterator first, InputIterator last) {
    clear();
    for (; first != last; ++first) {
        push_back(*first);
    }
}

template <typename T, typename A>
vector<T, A>& vector<T, A>::operator=(vector const& other) {
    if (this != &other) {
        base::copy_assign_alloc(other);
        assign(other.begin_, other.end_);
    }
    return *this;

    /*
        if (capacity() < other.size()) {
            vector tmp(other);
            std::swap(*this, tmp);
            return *this;
        }

        if (this == &other)
            return *this;

        base_.allocator_ = other.allocator_;
        if (other.size() < size()) {
            copy(other.begin(), other.end(), begin());
            for (iterator it = begin() + other.size(); it != capacity_end(); ++it) {
                it->~T();
            }
        } else {
        }
        */
}

template <typename T, typename A>
vector<T, A>::vector(vector&& other) noexcept : base(std::move(other)) {}

template <typename T, typename A>
vector<T, A>& vector<T, A>::operator=(vector&& other) noexcept {
    clear();
    std::swap(*this, other);
    return *this;
}

template <typename T, typename A>
vector<T, A>::~vector() {
    destroy_elements();
}

template <typename T, typename A>
typename vector<T, A>::iterator vector<T, A>::begin() noexcept {
    return iterator(begin_);
}
template <typename T, typename A>
typename vector<T, A>::const_iterator vector<T, A>::begin() const noexcept {
    return const_iterator(begin_);
}
template <typename T, typename A>
typename vector<T, A>::iterator vector<T, A>::end() noexcept {
    return iterator(end_);
}
template <typename T, typename A>
typename vector<T, A>::const_iterator vector<T, A>::end() const noexcept {
    return const_iterator(end_);
}
template <typename T, typename A>
typename vector<T, A>::iterator vector<T, A>::capacity_end() noexcept {
    return iterator(capacity_);
}
template <typename T, typename A>
typename vector<T, A>::const_iterator vector<T, A>::capacity_end() const noexcept {
    return const_iterator(capacity_);
}

template <typename T, typename A>
void vector<T, A>::destroy_elements() noexcept {
    for (pointer it = begin_; it != capacity_; ++it) {
        allocator_.destroy(it);
    }
    capacity_ = begin_;
}

template <typename T, typename A>
void vector<T, A>::reserve(size_type const n) {
    // clang-format off
    if (capacity() <= n) return;
    base tmp(allocator_, n);
    for (pointer it = tmp.begin_; it != end_; ++it) {
        allocator_.construct(it, std::move(*begin_));
        it->~value_type();
    }
    begin_    = tmp.begin_;
    capacity_ = tmp.capacity_;
    end_      = tmp.end_;
    // clang-format on
}

template <typename T, typename A>
void vector<T, A>::clear() noexcept {
    base::clear();
}

template <typename T, typename A>
void vector<T, A>::push_back(value_type const& value) {
    if (capacity() <= size()) {
        reserve(size() ? 2 * size() : 8);
    }
    allocator_.construct(&begin_[size()], value);
    ++end_;
}

template <typename T, typename A>
typename vector<T, A>::size_type vector<T, A>::size() const noexcept {
    return static_cast<size_type>(end_ - begin_);
}

template <typename T, typename A>
typename vector<T, A>::size_type vector<T, A>::capacity() const noexcept {
    return capacity_ - begin_;
}

template <typename T, typename A>
bool vector<T, A>::empty() const noexcept {
    return end_ == begin_;
}

template <typename T, typename A>
typename vector<T, A>::reference vector<T, A>::operator[](size_type const n) {
    assert(n <= size());
    return const_cast<reference>(static_cast<vector<T, A> const&>(*this)[n]);
}

template <typename T, typename A>
typename vector<T, A>::const_reference vector<T, A>::operator[](size_type const n) const {
    assert(n <= size());
    return begin_[n];
}

} // namespace etl
