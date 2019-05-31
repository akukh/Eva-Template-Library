#pragma once
#include <cstddef>

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
    struct two { char с[2]; };
    template <typename U> static two  test(...);
    template <typename U> static char test(typename U::iterator_category* = 0);
public:
    static bool const value = sizeof(test<T>(0)) == 1;
};

namespace iterator_traits_impl {

template <typename Iterator, bool>
struct iterator_traits_impl {};

template <typename Iterator>
struct iterator_traits_impl<Iterator, true> {
    typedef typename Iterator::difference_type   difference_type;
    typedef typename Iterator::value_type        value_type;
    typedef typename Iterator::pointer           pointer;
    typedef typename Iterator::reference         reference;
    typedef typename Iterator::iterator_category iterator_category;
};

template <typename Iterator, bool>
struct iterator_traits {};

template <typename Iterator>
struct iterator_traits<Iterator, true> : iterator_traits_impl<Iterator,
                                                              // TODO:
                                                              //  implement is_convertible, now here just stab
                                                              true> {};

} // namespace iterator_traits_impl

template <typename Iterator>
struct iterator_traits : iterator_traits_impl::iterator_traits<Iterator, has_iterator_category<Iterator>::value> {};

template <typename T>
struct iterator_traits<T*> {
    typedef ptrdiff_t                  difference_type;
    typedef T                          value_type; // typedef typename remove_cv<_Tp>::type value_type;
    typedef T*                         pointer;
    typedef T&                         reference;
    typedef random_access_iterator_tag iterator_category;
};

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

    // TODO:
    //  implement rest ctors
    wrap_iter() noexcept;
    wrap_iter(iterator_type other) noexcept;

    iterator_type base() const noexcept;

    reference operator* () const noexcept;
    pointer   operator->() const noexcept;

    wrap_iter& operator++() noexcept;
    wrap_iter& operator--() noexcept;

    wrap_iter operator+(difference_type const n) const noexcept;
    wrap_iter operator-(difference_type const n) const noexcept;

    wrap_iter& operator+=(difference_type const n) const noexcept;
    wrap_iter& operator-=(difference_type const n) const noexcept;

    reference operator[](difference_type const n) const noexcept;

private:
    iterator_type iterator_;

    template <typename Iterator1, typename Iterator2>
    friend bool operator==(wrap_iter<Iterator1> const&, wrap_iter<Iterator2> const&) noexcept;

    template <typename Iterator1, typename Iterator2>
    friend bool operator!=(wrap_iter<Iterator1> const&, wrap_iter<Iterator2> const&) noexcept;

    template <typename Iterator1, typename Iterator2>
    friend bool operator<(wrap_iter<Iterator1> const&, wrap_iter<Iterator2> const&) noexcept;

    template <typename Iterator1, typename Iterator2>
    friend bool operator>(wrap_iter<Iterator1> const&, wrap_iter<Iterator2> const&) noexcept;
};

} // namespace etl

// Implementation:
namespace etl {

template <typename Iterator>
wrap_iter<Iterator>::wrap_iter() noexcept : iterator_() {}

template <typename Iterator>
wrap_iter<Iterator>::wrap_iter(iterator_type other) noexcept : iterator_(other) {}

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
wrap_iter<Iterator>& wrap_iter<Iterator>::operator+=(difference_type const n) const noexcept {
    iterator_ += n;
    return *this;
}
template <typename Iterator>
wrap_iter<Iterator>& wrap_iter<Iterator>::operator-=(difference_type const n) const noexcept {
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

} // namespace etl
