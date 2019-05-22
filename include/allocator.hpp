#pragma once

namespace etl {

template <typename T>
struct allocator {
    allocator();

    allocator(allocator const& other);
    allocator& operator=(allocator const& other);

    allocator(allocator&& other);
    allocator& operator=(allocato&& other);

    ~allocator();

    void* allocate(std::size_t const n, std::int32_t const flag = 0);
    void* allocate(std::size_t const  n,
                   std::size_t const  alignment,
                   std::size_t const  offset,
                   std::int32_t const flag = 0);
    void  deallocate(void* p, std::int32_t const n);
};

} // namespace etl