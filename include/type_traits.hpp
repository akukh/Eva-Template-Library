#pragma once
#include <cstddef>

#include "config.hpp"

// clang-format off
namespace etl {

// Helper class:
template <typename T, T v>
struct integral_constant {
    typedef T                 value_type;
    typedef integral_constant type;

    static T CONSTEXPR const value = v;

#ifdef ETL_STD_VER_CXX03
    CONSTEXPR operator value_type() const NOEXCEPT { return value; }
#else
    CONSTEXPR value_type operator()() const NOEXCEPT { return value; }
#endif
};

template <typename T, T v>
T CONSTEXPR const integral_constant<T, v>::value;

typedef integral_constant<bool, true>  true_type;
typedef integral_constant<bool, false> false_type;

// Helper traits:
template <typename T> 
struct void_t { typedef void type; };

template <bool, typename T = void> struct enable_if          {};
template <typename T>              struct enable_if<true, T> { typedef T type; };

template <typename T1, typename T2> struct is_same       : false_type {};
template <typename T>               struct is_same<T, T> : true_type  {};

template <bool Flag, typename If, typename Then> struct conditional                  { typedef If type; };
template <typename If, typename Then>            struct conditional<false, If, Then> { typedef Then type; };

//----------------------------------------------------------------------------------------------------------------------
// Is reference
//----------------------------------------------------------------------------------------------------------------------
namespace details {

typedef char yes_type;
typedef char(&no_type)[8];

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

//----------------------------------------------------------------------------------------------------------------------
// Remove const volatile
//----------------------------------------------------------------------------------------------------------------------
template <typename T> struct remove_const          { typedef T type; };
template <typename T> struct remove_const<T const> { typedef T type; };

template <typename T> struct remove_volatile             { typedef T type; };
template <typename T> struct remove_volatile<T volatile> { typedef T type; };

template <typename T>
struct remove_cv { typedef typename remove_volatile<typename remove_const<T>::type>::type type; };

//----------------------------------------------------------------------------------------------------------------------
// Add reference
//----------------------------------------------------------------------------------------------------------------------
namespace details {

template <typename T, bool = is_reference<T>::value> struct add_lvalue_reference_impl          { typedef T  type; };
template <typename T>                                struct add_lvalue_reference_impl<T, true> { typedef T& type; };

template <typename T, bool = is_reference<T>::value> struct add_rvalue_reference_impl          { typedef T   type; };
template <typename T>                                struct add_rvalue_reference_impl<T, true> { typedef T&& type; };

} // namespace details 

template <typename T> 
struct add_lvalue_reference { typedef typename details::add_lvalue_reference_impl<T>::type type; };

template <typename T> 
struct add_rvalue_reference { typedef typename details::add_rvalue_reference_impl<T>::type type; };

//----------------------------------------------------------------------------------------------------------------------
// Is integral
//----------------------------------------------------------------------------------------------------------------------
namespace details {

template <typename T> struct is_integral_helper                     : false_type {};
template <>           struct is_integral_helper<bool>               : true_type  {};
template <>           struct is_integral_helper<char>               : true_type  {};
template <>           struct is_integral_helper<signed char>        : true_type  {};
template <>           struct is_integral_helper<unsigned char>      : true_type  {};
template <>           struct is_integral_helper<wchar_t>            : true_type  {};
template <>           struct is_integral_helper<short>              : true_type  {};
template <>           struct is_integral_helper<unsigned short>     : true_type  {};
template <>           struct is_integral_helper<int>                : true_type  {};
template <>           struct is_integral_helper<unsigned int>       : true_type  {};
template <>           struct is_integral_helper<long>               : true_type  {};
template <>           struct is_integral_helper<unsigned long>      : true_type  {};
template <>           struct is_integral_helper<long long>          : true_type  {};
template <>           struct is_integral_helper<unsigned long long> : true_type  {};

} // namespace details

template <typename T>
struct is_integral : details::is_integral_helper<typename remove_cv<T>::type> {};

//----------------------------------------------------------------------------------------------------------------------
// Is floating-point
//----------------------------------------------------------------------------------------------------------------------
namespace details {

template <typename T> struct is_floating_point_helper              : public false_type {};
template <>           struct is_floating_point_helper<float>       : public true_type {};
template <>           struct is_floating_point_helper<double>      : public true_type {};
template <>           struct is_floating_point_helper<long double> : public true_type {};

} // namespace details

template <typename T> 
struct is_floating_point : details::is_floating_point_helper<typename remove_cv<T>::type> {};

//----------------------------------------------------------------------------------------------------------------------
// Is arithmetic
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
struct is_arithmetic : integral_constant<bool, is_integral<T>::value || is_floating_point<T>::value> {};

//----------------------------------------------------------------------------------------------------------------------
// Pointer traits
//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename U = void>
struct has_difference_type : false_type {};

template <typename T>
struct has_difference_type<T, typename void_t<typename T::difference_type>::type> : true_type {};

template <typename T, typename U = void>
struct has_element_type : false_type {};

template <typename T>
struct has_element_type<T, typename void_t<typename T::element_type>::type> : true_type {};

template <typename Pointer, bool = has_element_type<Pointer>::value>
struct pointer_traits_element_type;

template <typename Pointer> 
struct pointer_trairs_element_type { typedef typename Pointer::element_type type; };

template <typename T, typename U>
struct has_rebind {
private:
    template <typename C> static details::no_type  has_rebind_check(...);
    template <typename C> static details::yes_type has_rebind_check(typename C::template rebind<C>* = 0);

public:
    enum { value = sizeof(has_rebind_check<T>(0)) == 1 };
};

template <typename T, typename U, bool = has_rebind<T, U>::value>
struct pointer_traits_rebind { typedef typename T::template rebind<U>::other type; };

template <typename Pointer>
struct pointer_traits {
    typedef Pointer                                             pointer;
    typedef typename pointer_traits_element_type<pointer>::type element_type;

    template <typename U> 
    struct rebind { typedef typename pointer_traits_rebind<pointer, U>::type other; };
};

template <typename T>
struct pointer_traits<T*> {
    typedef T* pointer;
    typedef T  element_type;

    template <typename U> 
    struct rebind { typedef U* other; };
};

template <typename Pointer, bool = has_difference_type<Pointer>::value>
struct pointer_traits_difference_type { typedef ptrdiff_t type; };

template <typename Pointer>
struct pointer_traits_difference_type<Pointer, true> { typedef typename Pointer::difference_type type; };

template <typename From, typename To>
struct rebind_pointer { typedef typename pointer_traits<From>::template rebind<To>::other type; };

template <typename T, typename U = void>
struct has_pointer_type : false_type {};

template <typename T>
struct has_pointer_type<T, typename void_t<typename T::pointer>::type> : true_type {};

namespace details {

template <typename T, typename U, bool = has_pointer_type<U>::value>
struct pointer_type_impl { typedef typename U::pointer type; };

template <typename T, typename U>
struct pointer_type_impl<T, U, false> { typedef T* type; };

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
struct const_pointer { 
    typedef typename Allocator::const_pointer type; 
};

template <typename T, typename Pointer, typename Allocator>
struct const_pointer<T, Pointer, Allocator, false> {
    typedef typename pointer_traits<Pointer>::template rebind<T const>::other type;
};

//----------------------------------------------------------------------------------------------------------------------
// Type list
//----------------------------------------------------------------------------------------------------------------------
template <typename Head, typename Tail>
struct type_list {
    typedef Head head;
    typedef Tail tail;
};

struct nat {};

//----------------------------------------------------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------------------------------------------------
typedef type_list<unsigned char, 
            type_list<unsigned int, 
                type_list<unsigned long, 
                    type_list<unsigned long long, nat>
                >
            >
        > unsigned_types;

template <typename TypeList, size_t Size, bool = Size <= sizeof(typename TypeList::head)>
struct find_first;

template <typename Head, typename Tail, size_t Size>
struct find_first<type_list<Head, Tail>, Size, true> { typedef Tail type; };

template <typename Head, typename Tail, size_t Size>
struct find_first<type_list<Head, Tail>, Size, false> { typedef typename find_first<Tail, Size>::type type; };

namespace details {

template <typename T, bool = is_integral<T>::value /* TODO: || is_enum<T>::value */>
struct make_unsigned_helper {};

template <typename T>
struct make_unsigned_helper<T, true> { typedef typename find_first<unsigned_types, sizeof(T)>::type type; };

template <> struct make_unsigned_helper<bool              , true> {};
template <> struct make_unsigned_helper<signed   int      , true> { typedef unsigned int  type; };
template <> struct make_unsigned_helper<unsigned int      , true> { typedef unsigned int  type; };
template <> struct make_unsigned_helper<signed   long     , true> { typedef unsigned long type; };
template <> struct make_unsigned_helper<unsigned long     , true> { typedef unsigned long type; };
template <> struct make_unsigned_helper<signed   long long, true> { typedef unsigned long long type; };
template <> struct make_unsigned_helper<unsigned long long, true> { typedef unsigned long long type; };

} // namespace details

template <typename T>
struct make_unsigned { typedef typename details::make_unsigned_helper<T>::type type; };

template <typename T, typename U = void>
struct has_size_type : false_type {};

template <typename T>
struct has_size_type<T, typename void_t<typename T::size_type>::type> : true_type {};

template <typename Allocator, typename DiffType, bool = has_size_type<Allocator>::value>
struct size_type { typedef typename make_unsigned<DiffType>::type type; };

template <typename Allocator, typename DiffType>
struct size_type<Allocator, DiffType, true> { typedef typename Allocator::size_type type; };

//----------------------------------------------------------------------------------------------------------------------
// Declval
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename add_rvalue_reference<T>::type declval() NOEXCEPT;

//----------------------------------------------------------------------------------------------------------------------
// Is constructible
//----------------------------------------------------------------------------------------------------------------------
namespace details {

template <typename, typename T, typename Arg>
struct is_constructible_helper : false_type {};

template <typename T, typename Arg>
struct is_constructible_helper<void_t<decltype(T(declval<Arg>()))>, T, Arg> : true_type {};

} // namespace details 

template <typename T, typename Arg>
struct is_constructible : details::is_constructible_helper<void_t<T>, T, Arg> {};

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
    enum { value = sizeof(is_convertible_check<T2>(declval<T1>())) == 1 };
};

template <typename T>
struct is_convertible_helper<T, T> {
private:
    template <typename U> static yes_type is_convertible_check(U const&);
    template <typename U> static no_type  is_convertible_check(...);

public:
    enum { value = sizeof(is_convertible_check<T>(declval<T>())) == 1 };
};

} // namespace details

template <typename T1, typename T2> 
struct is_convertible : integral_constant<bool, details::is_convertible_helper<T1, T2>::value> {};

//----------------------------------------------------------------------------------------------------------------------
// Checks is some struct or class has a concrete field:
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
    enum { value = sizeof(has_x_field_check<T>(0)) == 2 };
};

} // namespace details

} // namespace etl
