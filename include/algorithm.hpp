#pragma once
#include "iterator.hpp"

namespace etl {

template <typename InputIterator, class OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last, OutputIterator d_first) {
    while (first != last) {
        *d_first++ = *first++;
    }
    return d_first;
}

template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator d_first) {
    typedef typename etl::iterator_traits<ForwardIterator>::value_type Value;

    ForwardIt current = d_first;
    try {
        for (; first != last; ++first, (void)++current) {
            ::new (static_cast<void*>(std::addressof(*current))) Value(*first);
        }
        return current;
    } catch (...) {
        for (; d_first != current; ++d_first) {
            d_first->~Value();
        }
        throw;
    }
}

} // namespace etl