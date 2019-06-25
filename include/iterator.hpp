#pragma once
#include <cstddef>

#include "type_traits.hpp"

// clang-format off
namespace etl {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename T>
struct has_iterator_category {
private:
    template <typename U> static details::no_type  has_iterator_category_check(...);
    template <typename U> static details::yes_type has_iterator_category_check(typename U::iterator_category* = 0);

public:
    enum { value = sizeof(has_iterator_category_check<T>(0)) == 1 };
};

namespace details {

template <typename Iterator, bool>
struct iterator_traits_basic_impl {};

template <typename Iterator>
struct iterator_traits_basic_impl<Iterator, true> {
    typedef typename Iterator::difference_type   difference_type;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
    typedef typename Iterator::iterator_category iterator_category;
};

template <typename Iterator, bool>
struct iterator_traits_impl {};

template <typename Iterator>
struct iterator_traits_impl<Iterator, true> 
    : iterator_traits_basic_impl<Iterator,
                                is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
                                is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value
                                > {};

} // namespace details

template <typename Iterator>
struct iterator_traits : details::iterator_traits_impl<Iterator, has_iterator_category<Iterator>::value> {};

template <typename T>
struct iterator_traits<T*> {
    typedef ptrdiff_t                   difference_type;
    typedef typename remove_cv<T>::type value_type;
    typedef T*                          pointer;
    typedef T&                          reference;
    typedef random_access_iterator_tag  iterator_category;
};

template <typename T, typename U, bool = has_iterator_category<iterator_traits<T> >::value>
struct has_iterator_category_convertible_to
    : integral_constant<bool, is_convertible<typename iterator_traits<T>::iterator_category, U>::value> {};

template <typename T, typename U>
struct has_iterator_category_convertible_to<T, U, false> : false_type {};

template <typename T>
struct is_input_iterator : has_iterator_category_convertible_to<T, input_iterator_tag> {};

template <typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*, typename Reference = T&>
struct iterator {
    typedef T         value_type;
    typedef Distance  difference_type;
    typedef Pointer   pointer;
    typedef Reference reference;
    typedef Category  iterator_category;
};

template <typename Iterator>
struct wrap_iter {
    typedef Iterator                                                   iterator_type;
    typedef typename iterator_traits<iterator_type>::iterator_category iterator_category;
    typedef typename iterator_traits<iterator_type>::value_type        value_type;
    typedef typename iterator_traits<iterator_type>::difference_type   difference_type;
    typedef typename iterator_traits<iterator_type>::pointer           pointer;
    typedef typename iterator_traits<iterator_type>::reference         reference;

    wrap_iter() noexcept;
    wrap_iter(iterator_type other) noexcept;

    template <typename OtherIterator>
    wrap_iter(wrap_iter<OtherIterator> const& other);

    iterator_type base() const noexcept;

    reference operator* () const noexcept;
    pointer   operator->() const noexcept;

    wrap_iter& operator++() noexcept;
    wrap_iter& operator--() noexcept;

    wrap_iter operator+(difference_type const n) const noexcept;
    wrap_iter operator-(difference_type const n) const noexcept;

    wrap_iter& operator+=(difference_type const n) noexcept;
    wrap_iter& operator-=(difference_type const n) noexcept;

    reference operator[](difference_type const n) const noexcept;

private:
    iterator_type iterator_;

    template <typename U> friend struct wrap_iter;
    template <typename T, typename A> friend class vector;

    template <typename Iterator1, typename Iterator2>
    friend bool operator==(wrap_iter<Iterator1> const&, wrap_iter<Iterator2> const&) noexcept;

    template <typename Iterator1, typename Iterator2>
    friend bool operator!=(wrap_iter<Iterator1> const&, wrap_iter<Iterator2> const&) noexcept;

    template <typename Iterator1, typename Iterator2>
    friend bool operator<(wrap_iter<Iterator1> const&, wrap_iter<Iterator2> const&) noexcept;

    template <typename Iterator1, typename Iterator2>
    friend bool operator>(wrap_iter<Iterator1> const&, wrap_iter<Iterator2> const&) noexcept;

    template <typename Iterator1, typename Iterator2>
    friend typename wrap_iter<Iterator1>::difference_type operator-(wrap_iter<Iterator1> const&, 
                                                                    wrap_iter<Iterator2> const&) noexcept;

    template <typename Iterator1>
    friend wrap_iter<Iterator1> operator+(typename wrap_iter<Iterator1>::difference_type, 
                                          wrap_iter<Iterator1>) noexcept;
};

} // namespace etl

// Implementation:
namespace etl {

template <typename Iterator>
wrap_iter<Iterator>::wrap_iter() noexcept : iterator_() {}

template <typename Iterator>
wrap_iter<Iterator>::wrap_iter(iterator_type other) noexcept : iterator_(other) {}

template <typename Iterator>
template <typename OtherIterator>
wrap_iter<Iterator>::wrap_iter(wrap_iter<OtherIterator> const& other) : iterator_(other.base()) {}

template <typename Iterator>
typename wrap_iter<Iterator>::iterator_type wrap_iter<Iterator>::base() const noexcept {
    return iterator_;
}

template <typename Iterator>
typename wrap_iter<Iterator>::reference wrap_iter<Iterator>::operator*() const noexcept {
    return *iterator_;
}

template <typename Iterator>
typename wrap_iter<Iterator>::pointer wrap_iter<Iterator>::operator->() const noexcept {
    return pointer(&*iterator_);
}
template <typename Iterator>
wrap_iter<Iterator> wrap_iter<Iterator>::operator+(difference_type const n) const noexcept {
    wrap_iter tmp(*this);
    tmp += n;
    return tmp;
}
template <typename Iterator>
wrap_iter<Iterator> wrap_iter<Iterator>::operator-(difference_type const n) const noexcept {
    return *this + (-n);
}
template <typename Iterator>
wrap_iter<Iterator>& wrap_iter<Iterator>::operator+=(difference_type const n) noexcept {
    iterator_ += n;
    return *this;
}
template <typename Iterator>
wrap_iter<Iterator>& wrap_iter<Iterator>::operator-=(difference_type const n) noexcept {
    *this += (-n); 
    return *this;
}

template <typename Iterator>
typename wrap_iter<Iterator>::reference wrap_iter<Iterator>::operator[](difference_type const n) const noexcept {
    return iterator_[n];
}

template <typename Iterator>
wrap_iter<Iterator>& wrap_iter<Iterator>::operator++() noexcept {
    ++iterator_;
    return *this;
}

template <typename Iterator>
wrap_iter<Iterator>& wrap_iter<Iterator>::operator--() noexcept {
    ++iterator_;
    return *this;
}

template <typename Iterator1, typename Iterator2>
inline bool operator==(wrap_iter<Iterator1> const& x, wrap_iter<Iterator2> const& y) noexcept {
    return x.base() == y.base();
}

template <typename Iterator1, typename Iterator2>
inline bool operator<(wrap_iter<Iterator1> const& x, wrap_iter<Iterator2> const& y) noexcept {
    return x.base() < y.base();
}

template <typename Iterator1, typename Iterator2>
inline bool operator!=(wrap_iter<Iterator1> const& x, wrap_iter<Iterator2> const& y) noexcept {
    return !(x == y);
}

template <typename Iterator1, typename Iterator2>
inline bool operator>(wrap_iter<Iterator1> const& x, wrap_iter<Iterator2> const& y) noexcept {
    return y < x;
}

template <typename Iterator1, typename Iterator2>
inline typename wrap_iter<Iterator1>::difference_type operator-(wrap_iter<Iterator1> const& x,
                                                                wrap_iter<Iterator2> const& y) noexcept {
    return x.base() - y.base();
}

template <typename Iterator1>
inline wrap_iter<Iterator1> operator+(typename wrap_iter<Iterator1>::difference_type n, 
                                      wrap_iter<Iterator1>                           x) noexcept {
    x += n;
    return x;
}

} // namespace etl
