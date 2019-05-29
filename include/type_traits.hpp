#pragma once
#include <cstddef>

// clang-format off
namespace etl {

// Helper class:
template <typename T, T v>
struct integral_constant {
    typedef T                 value_type;
    typedef integral_constant type;

    static T constexpr const value = v;

    value_type constexpr operator()() const noexcept { return value; }
};

template <typename T, T v>
T constexpr const integral_constant<T, v>::value;

typedef integral_constant<bool, true>  true_type;
typedef integral_constant<bool, false> false_type;

// Helper traits:
template <bool, typename T = void>
struct enable_if {};
template <typename T>
struct enable_if<true, T> { typedef T type; };

template <typename T>
struct void_t { typedef void type; };

//----------------------------------------------------------------------------------------------------------------------
// Trait to check is some struct or class has a concrete field:
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
struct has_x_field {
private:
    struct two { char с[2]; };
    template <typename C, C> struct ch_t;
    // NOTE: if C has a member named x the second `test` function will be chosen,
    // otherwise the one with the elipsis will be chosen.
    template <typename C>             static two  test(...);
    template <typename C, typename U> static char test(ch_t<U C::*, &C::x>* = 0);
public:
    static bool const value = sizeof(test<T>(0)) == 2;
};

//----------------------------------------------------------------------------------------------------------------------
// Allocator
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
// Remove reference
//----------------------------------------------------------------------------------------------------------------------
template <typename T> struct remove_reference      { typedef T type; };
template <typename T> struct remove_reference<T&>  { typedef T type; };
template <typename T> struct remove_reference<T&&> { typedef T type; };

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

//----------------------------------------------------------------------------------------------------------------------
// Pointer traits
//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename U = void>
struct has_element_type : false_type {};
template <typename T>
struct has_element_type<T, typename void_t<typename T::element_type>::type> : true_type {};

template <typename Pointer, bool = has_element_type<Pointer>::value>
struct pointer_traits_element_type;
template <typename Pointer>
struct pointer_trairs_element_type {
    typedef typename Pointer::element_type type;
};

template <typename T, typename U>
struct has_rebind {
private:
    struct two { char с[2]; };
    template <typename C> static two  test(...);
    template <typename C> static char test(typename C::template rebind<C>* = 0);
public:
    static bool const value = sizeof(test<T>(0)) == 1;
};

template <typename T, typename U, bool = has_rebind<T, U>::value>
struct pointer_traits_rebind {
    typedef typename T::template rebind<U>::other type;
};

template <typename Pointer>
struct pointer_traits {
    typedef Pointer                                             pointer;
    typedef typename pointer_traits_element_type<pointer>::type element_type;

    template <typename U> struct rebind {
        typedef typename pointer_traits_rebind<pointer, U>::type other;
    };
};
template <typename T>
struct pointer_traits<T*> {
    typedef T* pointer;
    typedef T  element_type;

    template <typename U> struct rebind { typedef U* other; };
};

template <typename From, typename To>
struct rebind_pointer {
    typedef typename pointer_traits<From>::template rebind<To>::other type;
};

template <typename T, typename U = void>
struct has_pointer_type : false_type {};
template <typename T>
struct has_pointer_type<T, typename void_t<typename T::pointer>::type> : true_type {};

namespace pointer_type_impl {

template <typename T, typename U, bool = has_pointer_type<U>::value>
struct pointer_type {
    typedef typename U::pointer type;
};

template <typename T, typename U>
struct pointer_type<T, U, false> {
    typedef T* type;
};

} // namespace poiter_type_impl

template <typename T, typename U>
struct pointer_type {
    typedef typename pointer_type_impl::pointer_type<T, typename remove_reference<U>::type>::type type;
};

//----------------------------------------------------------------------------------------------------------------------
// Const pointer
//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename U = void>
struct has_const_pointer : false_type {};

template <typename T>
struct has_const_pointer<T, typename void_t<typename T::const_pointer>::type> : true_type {};

template <typename T, typename Pointer, typename Allocator, bool = has_const_pointer<Allocator>::value>
struct const_pointer { typedef typename Allocator::const_pointer type; };

template <typename T, typename Pointer, typename Allocator>
struct const_pointer<T, Pointer, Allocator, false> {
    typedef typename pointer_traits<Pointer>::template rebind<T const>::other type;
};

//----------------------------------------------------------------------------------------------------------------------
// Type list
//----------------------------------------------------------------------------------------------------------------------
template <typename H, typename T>
struct type_list {
    typedef H head;
    typedef T tail;
};

struct nat {};

//----------------------------------------------------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------------------------------------------------
typedef type_list<unsigned char, 
            type_list<unsigned int, 
                type_list<unsigned long, nat>
            >
        > unsigned_types;

//----------------------------------------------------------------------------------------------------------------------
// Find first
//----------------------------------------------------------------------------------------------------------------------
template <typename TypeList, size_t Size, bool = Size <= sizeof(typename TypeList::head)>
struct find_first;

template <typename H, typename T, size_t Size>
struct find_first<type_list<H, T>, Size, true> {
    typedef T type;
};

template <typename H, typename T, size_t Size>
struct find_first<type_list<H, T>, Size, false> {
    typedef typename find_first<T, Size>::type type;
};

//----------------------------------------------------------------------------------------------------------------------
// Make unsigned
//----------------------------------------------------------------------------------------------------------------------
namespace make_unsigned_impl {

template <typename T, bool = true /* is_integral<T>::value || is_enum<Tp>::value */>
struct make_unsigned {};

template <typename T>
struct make_unsigned<T, true> {
    typedef typename find_first<unsigned_types, sizeof(T)>::type type;
};

template <> struct make_unsigned<bool,          true> {};
template <> struct make_unsigned<signed   int,  true> { typedef signed   int  type; };
template <> struct make_unsigned<unsigned int,  true> { typedef unsigned int  type; };
template <> struct make_unsigned<signed   long, true> { typedef signed   long type; };
template <> struct make_unsigned<unsigned long, true> { typedef unsigned long type; };

} // namespace make_unsigned_impl


template <typename T>
struct make_unsigned {
    typedef typename make_unsigned_impl::make_unsigned<T>::type type;
};

//----------------------------------------------------------------------------------------------------------------------
// Size type
//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename U = void>
struct has_size_type : false_type {};

template <typename T>
struct has_size_type<T, typename void_t<typename T::size_type>::type> : true_type {};

template <typename Allocator, typename DiffType, bool = has_size_type<Allocator>::value>
struct size_type {
    typedef typename make_unsigned<DiffType>::type type;
};

template <typename Allocator, typename DiffType>
struct size_type<Allocator, DiffType, true> {
    typedef typename Allocator::size_type type;
};

//----------------------------------------------------------------------------------------------------------------------
// Difference type
//----------------------------------------------------------------------------------------------------------------------
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

template <class Allocator, class Pointer, bool = has_difference_type<Allocator>::value>
struct alloc_traits_difference_type {
    typedef typename pointer_traits<Pointer>::difference_type type;
};

template <class Allocator, class Pointer>
struct alloc_traits_difference_type<Allocator, Pointer, true> {
    typedef typename Allocator::difference_type type;
};

//----------------------------------------------------------------------------------------------------------------------
// Allocator traits
//----------------------------------------------------------------------------------------------------------------------
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
};

} // namespace etl
