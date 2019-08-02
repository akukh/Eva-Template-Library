#pragma once
#include <cstdint>

#include "type_traits.hpp"

// clang-format off
namespace etl {

template <typename T>
struct allocator;

template <>
struct allocator<void> {
    typedef void*       pointer;
    typedef void const* const_pointer;
    typedef void        value_type;

    template <typename U>
    struct rebind { typedef allocator<U> other; };
};

template <>
struct allocator<void const> {
    typedef void const* pointer;
    typedef void const* const_pointer;
    typedef void const  value_type;

    template <typename U>
    struct rebind { typedef allocator<U> other; };
};

template <typename Allocator, typename Pointer, bool = has_difference_type<Allocator>::value>
struct alloc_traits_difference_type {
    typedef typename pointer_traits<Pointer>::difference_type type;
};

template <typename Allocator, typename Pointer>
struct alloc_traits_difference_type<Allocator, Pointer, true> {
    typedef typename Allocator::difference_type type;
};

template <typename Allocator>
struct allocator_traits {
    typedef Allocator                                                            allocator_type;
    typedef typename allocator_type::value_type                                  value_type;
    typedef typename pointer_type<value_type, allocator_type>::type              pointer;
    typedef typename const_pointer<value_type, pointer, allocator_type>::type    const_pointer;
    typedef typename alloc_traits_difference_type<allocator_type, pointer>::type difference_type;
    typedef typename size_type<allocator_type, difference_type>::type            size_type;

    static pointer allocate(allocator_type& a, size_type n);
    static void    deallocate(allocator_type& a, pointer p, size_type n) NOEXCEPT;

    static void construct(allocator_type& a, pointer p, value_type const& value);
    static void destroy(allocator_type& a, pointer p);
};

template <typename T>
struct allocator {
    typedef T              value_type;
    typedef value_type*    pointer;
    typedef size_t         size_type;
    typedef ptrdiff_t      difference_type;

    CONSTEXPR allocator() NOEXCEPT {}
    CONSTEXPR allocator(allocator const&) NOEXCEPT {}
    template <typename U> 
    CONSTEXPR allocator(allocator<U> const&) NOEXCEPT {}

    NODISCARD pointer allocate(size_type const n);
    void              deallocate(pointer p, MAYBE_UNUSED size_type const n) NOEXCEPT;

    template <typename U, typename... Args> 
    void construct(U* p, Args&&... args);
    template <typename U>
    void destroy(U* p) NOEXCEPT;

    size_type max_size() const NOEXCEPT;
};

template <typename T, typename U>
inline bool operator==(const allocator<T>&, const allocator<U>&) NOEXCEPT { return true; }

template <typename T, typename U>
inline bool operator!=(const allocator<T>&, const allocator<U>&) NOEXCEPT { return false; }

} // namespace etl

// Allocator implementation:
namespace etl {

template <typename T>
NODISCARD typename allocator<T>::pointer allocator<T>::allocate(size_type const n) {
    if (n > max_size()) {
        throw "bad_alloc"; // TODO: throw bad_alloc()
    }
    return static_cast<pointer>(::operator new(n * sizeof(T)));
}

template <typename T>
void allocator<T>::deallocate(pointer p, MAYBE_UNUSED size_type const n) NOEXCEPT {
    ::operator delete(p);
}

template <typename T>
template <typename U, typename... Args>
void allocator<T>::construct(U* p, Args&&... args) {
    ::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
}

template <typename T>
template <typename U>
void allocator<T>::destroy(U* p) NOEXCEPT {
    p->~U();
}

template <typename T>
typename allocator<T>::size_type allocator<T>::max_size() const NOEXCEPT {
    return size_type(~0) / sizeof(value_type);
}

} // namespace etl
// Allocator implementation

// Allocator traits implementation:
namespace etl {

template <typename Allocator>
typename allocator_traits<Allocator>::pointer allocator_traits<Allocator>::allocate(allocator_type& a, size_type n) { 
    return a.allocate(n); 
}

template <typename Allocator>
void allocator_traits<Allocator>::deallocate(allocator_type& a, pointer p, size_type n) NOEXCEPT { 
    a.deallocate(p, n); 
}

template <typename Allocator>
void allocator_traits<Allocator>::construct(allocator_type& a, pointer p, value_type const& value) { 
    a.construct(p, value); 
}

template <typename Allocator>
void allocator_traits<Allocator>::destroy(allocator_type& a, pointer p) { 
    a.destroy(p); 
}

} // namespace etl
// Allocator traits implementation
