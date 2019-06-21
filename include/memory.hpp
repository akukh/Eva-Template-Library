#pragma once
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

template <typename T, typename U = void>
struct has_difference_type : false_type {};

template <typename T>
struct has_difference_type<T, typename void_t<typename T::difference_type>::type> : true_type {};

template <typename Pointer, bool = has_difference_type<Pointer>::value>
struct pointer_traits_difference_type {
    typedef ptrdiff_t type;
};

template <typename Pointer>
struct pointer_traits_difference_type<Pointer, true> {
    typedef typename Pointer::difference_type type;
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
    typedef Allocator                           allocator_type;
    typedef typename allocator_type::value_type value_type;

    typedef typename pointer_type<value_type, allocator_type>::type           pointer;
    typedef typename const_pointer<value_type, pointer, allocator_type>::type const_pointer;

    typedef typename alloc_traits_difference_type<allocator_type, pointer>::type difference_type;
    typedef typename size_type<allocator_type, difference_type>::type            size_type;

    // TODO:
    //  complete implementation
#if 0
    template <typename T> struct rebind_alloc {
        typedef typename allocator_traits_rebind<allocator_type, T>::type other;
    };
    template <typename T> struct rebind_traits {
        typedef allocator_traits<typename rebind_alloc<T>::other> other;
    };
#endif

    static pointer allocate(allocator_type& a, size_type n) { return a.allocate(n); }
    static void    deallocate(allocator_type& a, pointer p, size_type n) noexcept { a.deallocate(p, n); }

    static void construct(allocator_type& a, pointer p, value_type const& value) { a.construct(p, value); }
    static void destroy(allocator_type& a, pointer p) { a.destroy(p); }
};

} // namespace etl