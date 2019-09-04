#pragma once
#include <cassert>

#include "iterator.hpp"
#include "memory.hpp"

#include <cxxabi.h>
#include <exception>
#include <iostream>
#include <typeinfo>

namespace etl {

template <typename Type, typename Allocator>
struct vector_base {
    // protected:
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

    vector_base(allocator_type const& allocator, size_type const n);
    ~vector_base();

    vector_base(vector_base const& other) DELETE;
    vector_base& operator=(vector_base const& other) DELETE;

    vector_base(vector_base&& other) NOEXCEPT;
    vector_base& operator=(vector_base&& other) NOEXCEPT;

    void clear() NOEXCEPT;

    void construct_at_end(size_type const n, const_reference value) NOEXCEPT;
    void destruct_at_end(pointer new_end) NOEXCEPT;

    void copy_assign_alloc(vector_base const& other);

    void swap(vector_base& other);

    void relocate(size_type const size, pointer position);
    void relocate(size_type const size, size_type const offset, pointer position, const_reference value);

    void roll_elements_over(pointer position);
    void roll_elements_over(pointer position, const_reference value, size_type const offset, size_type const n = 1);
};

template <typename Type, typename Allocator = allocator<Type>>
class vector : private vector_base<Type, Allocator> {
    typedef vector_base<Type, Allocator> base;

public:
    typedef Type                           value_type;
    typedef Allocator                      allocator_type;
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

    vector(allocator_type const& allocator = Allocator()) NOEXCEPT;
    explicit vector(size_type const n, const_reference value = Type(), allocator_type const& allocator = Allocator());

    vector(vector const& other);
    vector& operator=(vector const& other);

    vector(vector&& other) NOEXCEPT;
    vector& operator=(vector&& other) NOEXCEPT;

    iterator       begin() NOEXCEPT;
    const_iterator begin() const NOEXCEPT;

    iterator       end() NOEXCEPT;
    const_iterator end() const NOEXCEPT;

    size_type size() const NOEXCEPT;
    size_type capacity() const NOEXCEPT;

    bool empty() const NOEXCEPT;

    void reserve(size_type const n);
    void resize(size_type const n, const_reference value = Type());

    void clear() NOEXCEPT;

    template <typename InputIterator>
    void assign(InputIterator first, InputIterator last);
    void assign(size_type const count, const_reference value);

    void push_back(const_reference value);

    // iterator insert(const_iterator position, value_type&& value);
    iterator insert(const_iterator position, const_reference value);
    iterator insert(const_iterator position, size_type const count, const_reference value);

    // clang-format off
    template <typename InputIterator>
    typename enable_if<
        is_input_iterator<InputIterator>::value &&
        is_constructible<
            value_type, 
            typename iterator_traits<InputIterator>::reference
        >::value,
        iterator
    >::type insert(const_iterator position, InputIterator first, InputIterator last);

    reference       operator[](size_type const n);
    const_reference operator[](size_type const n) const;

private:
    void destroy_elements() NOEXCEPT;

    void move_range_on(size_type const offset, pointer to);

    template <typename Filler = void (*)(pointer)>
    void reserve_n_fill(size_type const n, const_pointer position, Filler filler = Filler());
};

} // namespace etl

// vector_base implementation:
namespace etl {

template <typename Type, typename Allocator>
vector_base<Type, Allocator>::vector_base(allocator_type const& allocator, size_type const n)
    : allocator_(allocator), begin_(allocator_.allocate(n)), capacity_(begin_ + n), end_(begin_) {}

template <typename Type, typename Allocator>
vector_base<Type, Allocator>::~vector_base() {
    if (nullptr != begin_) {
        clear();
        allocator_.deallocate(begin_, capacity_ - begin_);
    }
}

template <typename Type, typename Allocator>
vector_base<Type, Allocator>::vector_base(vector_base&& other) NOEXCEPT 
    : allocator_(other.allocator_), begin_(other.begin_), capacity_(other.capacity_), end_(other.end_) {
    other.begin_ = other.end_ = other.capacity_ = nullptr;
}

template <typename Type, typename Allocator>
vector_base<Type, Allocator>& vector_base<Type, Allocator>::operator=(vector_base&& other) NOEXCEPT {
    std::swap(*this, other);
    return *this;
}

template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::clear() NOEXCEPT {
    destruct_at_end(begin_);
}

template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::construct_at_end(size_type const n, const_reference value) NOEXCEPT {
    for (std::size_t i = n; i--; end_++) {
        alloc_traits::construct(allocator_, end_, MOVE(value));
    }
}

template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::destruct_at_end(pointer new_end) NOEXCEPT {
    pointer soon_to_be_end = end_;
    while (soon_to_be_end-- > new_end) {
        int status;
        auto p = abi::__cxa_demangle(typeid(soon_to_be_end).name(), 0, 0, &status); ;
        (void)p;
        alloc_traits::destroy(allocator_, soon_to_be_end);
    }
    end_ = new_end;
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

template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::swap(vector_base& other) {
    std::swap(other.begin_, begin_);
    std::swap(other.end_, end_);
    std::swap(other.capacity_, capacity_);
}

// clang-format on
template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::relocate(size_type const size, pointer position) {
    assert(position && "must be non-nullptr");

    vector_base tmp(allocator_, size);
    alloc_traits::construct_forward(allocator_, position, end_, tmp.end_);
    swap(tmp);
}

template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::relocate(size_type const size,
                                            size_type const offset,
                                            pointer         position,
                                            const_reference value) {
    assert(position && "must be non-nullptr");

    vector_base new_buffer(allocator_, size);
    pointer     first = new_buffer.end_ = new_buffer.begin_ + offset;

    alloc_traits::construct(allocator_, new_buffer.end_++, value);
    alloc_traits::construct_backward(allocator_, begin_, position, first);
    alloc_traits::construct_forward(allocator_, position, end_, new_buffer.end_);
    swap(new_buffer);
}

template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::roll_elements_over(pointer position) {
    assert(position && "must be non-nullptr");

    size_type const size = static_cast<size_type>(end_ - begin_);
    vector_base     new_buffer(allocator_, size ? 2 * size : 8);

    alloc_traits::construct_forward(allocator_, position, end_, new_buffer.end_);
    swap(new_buffer);
}

template <typename Type, typename Allocator>
void vector_base<Type, Allocator>::roll_elements_over(pointer         position,
                                                      const_reference value,
                                                      size_type const offset,
                                                      size_type const n) {
    assert(position && "must be non-nullptr");

    size_type const size = static_cast<size_type>(end_ - begin_);
    vector_base     new_buffer(allocator_, size ? 2 * size : 8);
    pointer         first = new_buffer.end_ = new_buffer.begin_ + offset;

    new_buffer.construct_at_end(n, value);
    alloc_traits::construct_backward(allocator_, begin_, position, first);
    alloc_traits::construct_forward(allocator_, position, end_, new_buffer.end_);
    swap(new_buffer);
}
// clang-format off

} // namespace etl
// vector_base implementation end

// vector implementation:
namespace etl {

template <typename T, typename A>
vector<T, A>::vector(allocator_type const& allocator) NOEXCEPT : base(allocator, 8) {}

template <typename T, typename A>
vector<T, A>::vector(size_type const n, const_reference value, allocator_type const& allocator) : base(allocator, n) {
    try {
        int status;
        auto p = abi::__cxa_demangle(typeid(base::begin_).name(), 0, 0, &status); ;
        (void)p;
        for (size_type i = 0; i < n; ++i) {
            alloc_traits::construct(base::allocator_, base::end_++, MOVE(value));
        }
    } catch (...) {
        clear();
        throw;
    }
}

template <typename T, typename A>
vector<T, A>::vector(vector const& other) : base(other.allocator_, other.size()) {
    try {
        pointer first = other.begin_;
        pointer last  = other.end_;

        for (; first != last; ++first) {
            alloc_traits::construct(base::allocator_, base::end_++, *first);
        }
    } catch (...) {
        clear();
        throw;
    }
}

template <typename T, typename A>
vector<T, A>& vector<T, A>::operator=(vector const& other) {
    if (this != &other) {
        base::copy_assign_alloc(other);
        assign(other.begin_, other.end_);
    }
    return *this;
}

template <typename T, typename A>
vector<T, A>::vector(vector&& other) NOEXCEPT : base(MOVE(other)) {}

template <typename T, typename A>
vector<T, A>& vector<T, A>::operator=(vector&& other) NOEXCEPT {
    clear();
    std::swap(*this, other);
    return *this;
}

template <typename T, typename A>
typename vector<T, A>::iterator vector<T, A>::begin() NOEXCEPT {
    return iterator(base::begin_);
}

template <typename T, typename A>
typename vector<T, A>::const_iterator vector<T, A>::begin() const NOEXCEPT {
    return const_iterator(base::begin_);
}

template <typename T, typename A>
typename vector<T, A>::iterator vector<T, A>::end() NOEXCEPT {
    return iterator(base::end_);
}

template <typename T, typename A>
typename vector<T, A>::const_iterator vector<T, A>::end() const NOEXCEPT {
    return const_iterator(base::end_);
}

template <typename T, typename A>
typename vector<T, A>::size_type vector<T, A>::size() const NOEXCEPT {
    return static_cast<size_type>(base::end_ - base::begin_);
}

template <typename T, typename A>
typename vector<T, A>::size_type vector<T, A>::capacity() const NOEXCEPT {
    return static_cast<size_type>(base::capacity_ - base::begin_);
}

template <typename T, typename A>
bool vector<T, A>::empty() const NOEXCEPT {
    return base::end_ == base::begin_;
}

template <typename T, typename A>
void vector<T, A>::reserve(size_type const n) {
    if (n > capacity()) {
        base::relocate(n, base::begin_);
    }
}

template <typename T, typename A>
void vector<T, A>::resize(size_type const n, const_reference value) {
    if (size() < n) {
        if (capacity() >= n) {
            base::construct_at_end(n, value);
        } else {
            base::relocate(n, base::begin_);
            base::construct_at_end(n - size(), value);
        }
    } else if (size() > n){
        base::destruct_at_end(base::begin_ + n);
    }
}

template <typename T, typename A>
void vector<T, A>::clear() NOEXCEPT {
    base::clear();
}

template <typename T, typename A>
template <typename InputIterator>
void vector<T, A>::assign(InputIterator first, InputIterator last) {
    clear();
    insert(begin(), first, last);
}

template <typename T, typename A>
void vector<T, A>::assign(size_type const count, const_reference value) {
    clear();
    insert(begin(), count, value);
}

template <typename T, typename A>
void vector<T, A>::push_back(const_reference value) {
    insert(base::end_, value);
}
/*
template <typename T, typename A>
vector<T, A>::iterator vector<T, A>::insert(const_iterator position, value_type&& value) {}*/

template <typename T, typename A>
typename vector<T, A>::iterator vector<T, A>::insert(const_iterator position, const_reference value) {
    size_type const offset          = position - begin();
    pointer         insert_position = base::begin_ + offset;

    if (base::end_ < base::capacity_) {
        if (insert_position == base::end_) {
            alloc_traits::construct(base::allocator_, base::end_++, MOVE(value));
        } else {
            move_range_on(1, insert_position);
            alloc_traits::construct(base::allocator_, insert_position, MOVE(value));
        }
    } else {
        base::roll_elements_over(insert_position, value, offset);
    }

    return begin() + offset;
}

// clang-format on
template <typename T, typename A>
typename vector<T, A>::iterator vector<T, A>::insert(const_iterator  position,
                                                     size_type const count,
                                                     const_reference value) {
    assert(count > 0 && "must be greater than zero");

    size_type const offset          = position - begin();
    size_type const free_space      = base::capacity_ - base::end_;
    pointer         insert_position = base::begin_ + offset;

    if (1 == count)
        return insert(position, value);

    if (count <= free_space) {
        move_range_on(count, insert_position);
        for (size_type i = count; i--;) {
            alloc_traits::construct(base::allocator_, insert_position++, MOVE(value));
        }
    } else {
        base::roll_elements_over(insert_position, value, offset, count);
    }

    return begin() + offset;
}

// clang-format off
template <typename T, typename A>
template <typename InputIterator>
typename enable_if<
    is_input_iterator<InputIterator>::value &&
    is_constructible<
        typename vector<T, A>::value_type, 
        typename iterator_traits<InputIterator>::reference
    >::value,
    typename vector<T, A>::iterator
>::type vector<T, A>::insert(const_iterator position, InputIterator first, InputIterator last) {
    size_type const offset         = position - begin();
    size_type const required_space = last - first;
    size_type const free_space     = base::capacity_ - base::end_;
    pointer         p              = base::begin_ + offset;

    if (1 == required_space)
        return insert(position, *first);

    if (required_space <= free_space) {
        move_range_on(required_space, p);
        for (pointer it = p; it != p + required_space && first != last; ++first) {
            alloc_traits::construct(base::allocator_, it++, MOVE(*first));
            // base::allocator_.destroy(first);
        }
    } else {
        reserve_n_fill(required_space + size() + 8, p, [this, &first, &last](auto& end) {
            for (; first != last; ++first) {
                alloc_traits::construct(base::allocator_, end++, MOVE(*first));
                // base::allocator_.destroy(first);
            }
        });
    }
    return begin() + offset;
}

template <typename T, typename A>
typename vector<T, A>::reference vector<T, A>::operator[](size_type const n) {
    assert(n < size());
    return const_cast<reference>(static_cast<vector const&>(*this)[n]);
}

template <typename T, typename A>
typename vector<T, A>::const_reference vector<T, A>::operator[](size_type const n) const {
    assert(n < size());
    return base::begin_[n];
}

template <typename T, typename A>
void vector<T, A>::move_range_on(size_type const offset, pointer to) {
    /*  Example:
                      ↓ - from
        +---+---+---+---+---+---+---+
        | 5 | 1 | 3 | 7 | 0 | 0 | 0 |
        +---+---+---+---+---+---+---+
              ↑ - to              ↑ - last
     */

    assert(to && "must be non-nullptr");
    assert(base::end_ != to && "can't be the last element");

    pointer from = base::end_;
    pointer last = from + offset;

    while (from-- > to) {
        alloc_traits::construct(base::allocator_, --last, MOVE(*from));
        alloc_traits::destroy(base::allocator_, from);
    }
    base::end_ += offset;
}

template <typename T, typename A>
template <typename Filler>
void vector<T, A>::reserve_n_fill(size_type const n, const_pointer position, Filler filler) {
    if (capacity() > n) 
        return;

    base tmp(base::allocator_, n);
    for (pointer it = base::begin_; it != base::end_; ++it) {
        if (it == position) 
            filler(tmp.end_);
        alloc_traits::construct(base::allocator_, tmp.end_++, MOVE(*it));
        base::allocator_.destroy(it);
    }
    base::swap(tmp);
}

} // namespace etl
// vector implementation end
