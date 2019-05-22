#pragma once

namespace etl {

template <typename T, typename A = allocator<T>>
class vector {
public:
    explicit vector(std::size_t const n, T const& value = T(), A const& allocator = A());

    vector(vector const& other);
    vector& operator=(vector const& other);

    vector(vector&& other);
    vector& operator=(vector&& other);

    ~vector();

    std::size_t size() const noexcept;
    std::size_t capacity() const noexcept;

    void reserve(std::size_t const n);
    void resize(std::size_t const n, T const& value = ());

    void push_back(T const& value);
};

} // namespace etl