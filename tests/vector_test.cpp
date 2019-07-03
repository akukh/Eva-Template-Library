#include <catch2/catch.hpp>

#include "../include/vector.hpp"

namespace {
// clang-format off
// NOTE:
//  minimal allocator with debug output for Unit Tests only.
template <typename T> struct UT_allocator {
    typedef T              value_type;
    typedef value_type*    pointer;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;

    [[nodiscard]] pointer allocate(size_type n) {
        n *= sizeof(T);
        std::printf("[%p] allocating %lu bytes\n", this, n);
        return static_cast<pointer>(::operator new(n));
    }
    void deallocate(pointer p, size_type const n) {
        std::printf("[%p] deallocating %lu bytes\n", this, n * sizeof(*p));
        ::operator delete(p);
    }
    template <typename U, typename... Args> void construct(U* p, Args&&... args) {}
    template <typename U>                   void destroy(U* p) noexcept {}
};
// clang-format on
} // namespace

TEST_CASE("Vector creation.", "[vector]") {

    // Creating "empty" vector.
    {
        etl::vector<int> v;
        REQUIRE(0 == v.size());
        REQUIRE(0 == v.capacity());
    }

    // Creating a vector of 10 elements.
    {
        etl::vector<int> v(10);
        REQUIRE(10 == v.size());
        REQUIRE(10 <= v.capacity());
    }

    // Creating a vector of 10 elements and filled with value 0.
    {
        etl::vector<int> v(10, 0);
        for (auto& i : v) {
            REQUIRE(0 == i);
        }
        REQUIRE(10 == v.size());
        REQUIRE(10 <= v.capacity());
    }
}

TEST_CASE("Vector reserve.", "[vector]") {

    // Reserving space when vector is "empty".
    {
        etl::vector<int, UT_allocator<int>> v;
        std::printf("vector[%p] reserving %d points.\n", &v, 10);
        v.reserve(10);
        REQUIRE(0 == v.size());
        REQUIRE(10 == v.capacity());
    }

    // Reserving space when vector constructed with capacity.
    {
        std::printf("\n");
        etl::vector<int, UT_allocator<int>> v(10);
        std::printf("vector[%p] reserving %d points.\n", &v, 20);
        v.reserve(20);
        REQUIRE(10 == v.size());
        REQUIRE(20 == v.capacity());
    }

    // Reserving less space when vector constructed with capacity.
    {
        std::printf("\n");
        etl::vector<int, UT_allocator<int>> v(20);
        std::printf("vector[%p] reserving %d points.\n", &v, 20);
        v.reserve(10);
        REQUIRE(20 == v.size());
        REQUIRE(20 == v.capacity());
    }
}

TEST_CASE("Vector resize.", "[vector]") {

    {
        etl::vector<int> v;
        v.resize(10);
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }

    {
        etl::vector<int> v(5, 2019);
        std::printf("\nvector[%p] was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector[%p] contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        v.resize(10);
        std::printf("vector[%p] was resized to %lu\n", &v, v.size());
        std::printf("vector[%p] should contain additional elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }

    {
        etl::vector<int> v(3, 2019);
        std::printf("\nvector[%p] was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector[%p] contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        v.resize(10, 666);
        std::printf("vector[%p] was resized to %lu, value = %d\n", &v, v.size(), 666);
        std::printf("vector[%p] should contain additional elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }

    {
        etl::vector<int> v(10, 2019);
        std::printf("\nvector[%p] was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector[%p] contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        v.resize(5);
        std::printf("vector[%p] was resized to %lu\n", &v, v.size());
        std::printf("vector[%p] should contain fewer elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        REQUIRE(5 == v.size());
        REQUIRE(10 == v.capacity());
    }

    {
        etl::vector<int> v(10, 2019);
        std::printf("\nvector[%p] was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector[%p] contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        v.resize(5);
        std::printf("vector[%p] was resized to %lu\n", &v, v.size());
        std::printf("vector[%p] should contain fewer elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        REQUIRE(5 == v.size());
        REQUIRE(10 == v.capacity());
        v.resize(10, 666);
        std::printf("vector[%p] was resized to %lu, value = %d\n", &v, v.size(), 666);
        std::printf("vector[%p] should contain additional elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }
}
