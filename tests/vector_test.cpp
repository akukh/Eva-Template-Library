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

    {
        etl::vector<int> v;
        v.reserve(10);
        REQUIRE(0 == v.size());
        REQUIRE(10 == v.capacity());
    }

    {
        etl::vector<int> v(10);
        v.reserve(20);
        REQUIRE(10 == v.size());
        REQUIRE(20 == v.capacity());
    }
}

TEST_CASE("Vector resize.", "[vector]") {

    {
        etl::vector<int> v;
        v.resize(10);
        REQUIRE(10 == v.size());
        REQUIRE(10 <= v.capacity());
    }

    {
        etl::vector<int> v(5, 2019);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("\n");
        v.resize(10);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("\n");
        REQUIRE(10 == v.size());
        REQUIRE(10 <= v.capacity());
    }

    {
        etl::vector<int> v(3, 2019);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("\n");
        v.resize(10, 666);
        for (auto& i : v) {
            std::printf("%d ", i);
        }
        std::printf("\n");
        REQUIRE(10 == v.size());
        REQUIRE(10 <= v.capacity());
    }
}
