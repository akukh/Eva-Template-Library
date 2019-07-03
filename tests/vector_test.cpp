#include <catch2/catch.hpp>

#include "../include/vector.hpp"

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
        etl::vector<int> v;
        v.reserve(10);
        REQUIRE(0 == v.size());
        REQUIRE(10 == v.capacity());
    }

    // Reserving space when vector constructed with capacity.
    {
        etl::vector<int> v(10);
        v.reserve(20);
        REQUIRE(10 == v.size());
        REQUIRE(20 == v.capacity());
    }

    // Reserving less space when vector constructed with capacity.
    {
        etl::vector<int> v(20);
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
        std::printf("vector&%p was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector&%p contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        v.resize(10);
        std::printf("vector&%p was resized to %lu\n", &v, v.size());
        std::printf("vector&%p should contain additional elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }

    {
        etl::vector<int> v(3, 2019);
        std::printf("\nvector&%p was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector&%p contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        v.resize(10, 666);
        std::printf("vector&%p was resized to %lu, value = %d\n", &v, v.size(), 666);
        std::printf("vector&%p should contain additional elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }

    {
        etl::vector<int> v(10, 2019);
        std::printf("\nvector&%p was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector&%p contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        v.resize(5);
        std::printf("vector&%p was resized to %lu\n", &v, v.size());
        std::printf("vector&%p should contain fewer elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        REQUIRE(5 == v.size());
        REQUIRE(10 == v.capacity());
    }

    {
        etl::vector<int> v(10, 2019);
        std::printf("\nvector&%p was constructed with size = %lu, value = %d\n", &v, v.size(), v[0]);
        std::printf("vector&%p contains:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        v.resize(5);
        std::printf("vector&%p was resized to %lu\n", &v, v.size());
        std::printf("vector&%p should contain fewer elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        REQUIRE(5 == v.size());
        REQUIRE(10 == v.capacity());
        v.resize(10, 666);
        std::printf("vector&%p was resized to %lu, value = %d\n", &v, v.size(), 666);
        std::printf("vector&%p should contain additional elements:\n[ ", &v);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("]\n");
        REQUIRE(10 == v.size());
        REQUIRE(10 == v.capacity());
    }
}
