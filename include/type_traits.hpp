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

template <typename T1, typename T2> struct is_same       : false_type {};
template <typename T>               struct is_same<T, T> : true_type  {};

//----------------------------------------------------------------------------------------------------------------------
// Is reference
//----------------------------------------------------------------------------------------------------------------------
namespace details {

typedef char yes_type;
struct       no_type { char padding[8]; };

struct is_reference_impl {
    template <typename T> static T&      check(int);
    template <typename T> static no_type check(...);
};

} // namespace details 

template <typename T>
struct is_reference : integral_constant<bool, 
    !is_same<decltype(details::is_reference_impl::check<T>(0)), details::no_type>::value> {};

//----------------------------------------------------------------------------------------------------------------------
// Remove reference
//----------------------------------------------------------------------------------------------------------------------
template <typename T> struct remove_reference      { typedef T type; };
template <typename T> struct remove_reference<T&>  { typedef T type; };
template <typename T> struct remove_reference<T&&> { typedef T type; };

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

//----------------------------------------------------------------------------------------------------------------------
// Add reference
//----------------------------------------------------------------------------------------------------------------------
template <typename T, bool = is_reference<T>::value> struct add_lvalue_reference_impl          { typedef T  type; };
template <typename T                               > struct add_lvalue_reference_impl<T, true> { typedef T& type; };

template <typename T> struct add_lvalue_reference { 
    typedef typename add_lvalue_reference_impl<T>::type type; 
};

template <typename T> using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T, bool = is_reference<T>::value> struct add_rvalue_reference_impl          { typedef T   type; };
template <typename T                               > struct add_rvalue_reference_impl<T, true> { typedef T&& type; };

template <typename T> struct add_rvalue_reference {
    typedef typename add_rvalue_reference_impl<T>::type type;
};

template <typename T> using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

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
    template <typename C> static details::no_type  has_rebind_check(...);
    template <typename C> static details::yes_type has_rebind_check(typename C::template rebind<C>* = 0);

public:
    static bool const value = sizeof(has_rebind_check<T>(0)) == 1;
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

namespace details {

template <typename T, typename U, bool = has_pointer_type<U>::value>
struct pointer_type_impl {
    typedef typename U::pointer type;
};

template <typename T, typename U>
struct pointer_type_impl<T, U, false> {
    typedef T* type;
};

} // namespace details

template <typename T, typename U>
struct pointer_type {
    typedef typename details::pointer_type_impl<T, typename remove_reference<U>::type>::type type;
};

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

namespace details {

template <typename T, bool = true /* TODO: is_integral<T>::value || is_enum<Tp>::value */>
struct make_unsigned_helper {};

template <typename T>
struct make_unsigned_helper<T, true> {
    typedef typename find_first<unsigned_types, sizeof(T)>::type type;
};

template <> struct make_unsigned_helper<bool,          true> {};
template <> struct make_unsigned_helper<signed   int,  true> { typedef signed   int  type; };
template <> struct make_unsigned_helper<unsigned int,  true> { typedef unsigned int  type; };
template <> struct make_unsigned_helper<signed   long, true> { typedef signed   long type; };
template <> struct make_unsigned_helper<unsigned long, true> { typedef unsigned long type; };

} // namespace make_unsigned_impl

template <typename T>
struct make_unsigned {
    typedef typename details::make_unsigned_helper<T>::type type;
};

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
// Declval
//----------------------------------------------------------------------------------------------------------------------

template <typename T>
typename add_rvalue_reference<T>::type declval() noexcept;

//----------------------------------------------------------------------------------------------------------------------
// Is convertible
//----------------------------------------------------------------------------------------------------------------------
namespace details {

template <typename T1, typename T2>
struct is_convertible_helper {
private:
    template <typename U> static yes_type is_convertible_check(U const&);
    template <typename U> static no_type  is_convertible_check(...);

public:
    static bool const value = sizeof(is_convertible_check<T2>(declval<T1>())) == 1;
};

template <typename T>
struct is_convertible_helper<T, T> {
private:
    template <typename U> static yes_type is_convertible_check(U const&);
    template <typename U> static no_type  is_convertible_check(...);

public:
    static bool const value = sizeof(is_convertible_check<T>(declval<T>())) == 1;
};

} // namespace details

template <typename T1, typename T2> 
struct is_convertible : integral_constant<bool, details::is_convertible_helper<T1, T2>::value> {};

//----------------------------------------------------------------------------------------------------------------------
// Trait to check is some struct or class has a concrete field:
//----------------------------------------------------------------------------------------------------------------------
namespace details {

template <typename T>
struct has_x_field {
private:
    template <typename C, C> struct ch_t;
    // NOTE: if C has a member named x the second `test` function will be chosen,
    //  otherwise the one with the elipsis will be chosen.
    template <typename C, typename U> static yes_type has_x_field_check(ch_t<U C::*, &C::x>* = 0);
    template <typename C>             static no_type  has_x_field_check(...);
    
public:
    static bool const value = sizeof(has_x_field_check<T>(0)) == 2;
};

}

} // namespace etl
