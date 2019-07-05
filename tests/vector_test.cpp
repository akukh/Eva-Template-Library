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

SCENARIO("vectors can be constructed in different ways", "[vector]") {

    GIVEN("an empty vector of integer values") {
        etl::vector<int> v;
        WHEN("a vector instance is created") {
            THEN("its size and capacity must be equal to zero") {
                REQUIRE(0 == v.size());
                REQUIRE(0 == v.capacity());
            }
        }
    }

    GIVEN("an integer vector which contains ten elements") {
        etl::vector<int> v(10);
        WHEN("a vector instance is created") {
            THEN("its size and capacity must be equal to ten") {
                REQUIRE(10 == v.size());
                REQUIRE(10 == v.capacity());
            }
        }
    }

    GIVEN("an integer vector which contains twenty elements with a value=2019") {
        etl::vector<int> v(20, 2019);
        WHEN("a vector instance is created") {
            THEN("its size and capacity is equal to twenty") {
                REQUIRE(20 == v.size());
                REQUIRE(20 == v.capacity());
                AND_THEN("its value of elements must be equal to 2019") {
                    for (auto& i : v)
                        REQUIRE(2019 == i);
                }
            }
        }
    }

    GIVEN("an floating-point vector which contains five elements with a value=2019.0") {
        etl::vector<double> v1(5, 2019.0);

        REQUIRE(5 == v1.size());
        REQUIRE(5 == v1.capacity());

        AND_GIVEN("a vector constructed from some another vector") {
            WHEN("the original vector was copied") {
                AND_WHEN("a new vector constructed with the contents of other") {
                    etl::vector<double> v2(v1);
                    THEN("its size and capacity must be equal to the size and capacity of the original vector") {
                        REQUIRE(5 == v2.size());
                        REQUIRE(5 == v2.capacity());
                        AND_THEN("its value of elements must be equal to 2019.0") {
                            for (auto& i : v2)
                                REQUIRE(2019.0 == i);
                        }
                    }
                }
            }
        }
    }

    GIVEN("an floating-point vector which contains ten elements with a value=2019.0") {
        etl::vector<double> v1(10, 2019.0);

        REQUIRE(10 == v1.size());
        REQUIRE(10 == v1.capacity());

        AND_GIVEN("a vector constructed from some another vector using move semantics") {
            WHEN("the original vector was moved") {
                AND_WHEN("a new vector constructed with the contents of other") {
                    etl::vector<double> v2(std::move(v1));
                    THEN("its size and capacity must be equal to the size and capacity of the original vector") {
                        REQUIRE(10 == v2.size());
                        REQUIRE(10 == v2.capacity());
                        AND_THEN("its value of elements must be equal to 2019.0") {
                            for (auto& i : v2)
                                REQUIRE(2019.0 == i);
                        }
                    }
                }
                THEN("the original vector must be empty") { REQUIRE(v1.empty()); }
            }
        }
    }
}

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
        // Creating an "empty" vector with a special debug version of the allocator.
        etl::vector<int, UT_allocator<int>> v;
        std::printf("vector[%p] reserving %d points.\n", &v, 10);
        v.reserve(10);

        // Getting new values.
        REQUIRE(0 == v.size());
        REQUIRE(10 == v.capacity());
    }

    // Reserving space when vector constructed with capacity.
    {
        std::printf("\n");

        // Creating a vector of 10 elements with a special debug version of the allocator.
        etl::vector<int, UT_allocator<int>> v(10);
        std::printf("vector[%p] reserving %d points.\n", &v, 20);
        v.reserve(20);

        // Getting new values.
        REQUIRE(10 == v.size());
        REQUIRE(20 == v.capacity());
    }

    // Attempting to reserve less memory than there is originally.
    {
        std::printf("\n");

        // Creating a vector of 20 elements with a special debug version of the allocator.
        etl::vector<int, UT_allocator<int>> v(20);
        std::printf("vector[%p] reserving %d points.\n", &v, 20);
        v.reserve(10);

        // Values should not change.
        REQUIRE(20 == v.size());
        REQUIRE(20 == v.capacity());
    }
}

TEST_CASE("Vector resize.", "[vector]") {

    // Attempting to resize an "empty" vector.
    {
        // Creating "empty" vector.
        etl::vector<int> v;
        v.resize(10);

        // Getting new values.
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }

    // Attempting to resize a non-empty vector.
    {
        // Creating vector with 5 elements.
        etl::vector<int> v(5, 2019);
        std::printf("\nvector[%p] was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector[%p] contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");

        // Resizing vector to the new size.
        v.resize(10);
        std::printf("vector[%p] was resized to %lu\n", &v, v.size());
        std::printf("vector[%p] should contain additional elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");

        // Getting new values.
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }

    // Attempting to resize a non-empty vector with values "666".
    {
        // Creating vector with 3 elements.
        etl::vector<int> v(3, 2019);
        std::printf("\nvector[%p] was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector[%p] contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");

        // Resizing vector to the new size and fill with "666" values.
        v.resize(10, 666);
        std::printf("vector[%p] was resized to %lu, value = %d\n", &v, v.size(), 666);
        std::printf("vector[%p] should contain additional elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");

        // Getting new values.
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }

    // Attempting to resize a non-empty vector.
    {
        // Creating vector with 10 elements.
        etl::vector<int> v(10, 2019);
        std::printf("\nvector[%p] was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector[%p] contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");

        // Change the size of the vector to smaller.
        v.resize(5);
        std::printf("vector[%p] was resized to %lu\n", &v, v.size());
        std::printf("vector[%p] should contain fewer elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");

        // Getting new smaller values.
        REQUIRE(5 == v.size());
        REQUIRE(10 == v.capacity());
    }

    // Attempting to resize a non-empty vector twice.
    {
        // Creating vector with 10 elements.
        etl::vector<int> v(10, 2019);
        std::printf("\nvector[%p] was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector[%p] contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");

        // Change the size of the vector to smaller.
        v.resize(5);
        std::printf("vector[%p] was resized to %lu\n", &v, v.size());
        std::printf("vector[%p] should contain fewer elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");

        // Getting new smaller values.
        REQUIRE(5 == v.size());
        REQUIRE(10 == v.capacity());

        // Resize the vector back to a larger one and fill with "666" values.
        v.resize(10, 666);
        std::printf("vector[%p] was resized to %lu, value = %d\n", &v, v.size(), 666);
        std::printf("vector[%p] should contain additional elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");

        // Getting new "old" values.
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }
}
