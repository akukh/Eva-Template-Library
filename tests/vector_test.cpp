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

    GIVEN("an integer vector which already contains elements") {
        etl::vector<int> v(10);
        WHEN("a vector instance is created") {
            THEN("its size and capacity must be equal to ten") {
                REQUIRE(10 == v.size());
                REQUIRE(10 == v.capacity());
            }
        }
    }

    GIVEN("an integer vector which already contains elements with a value=2019") {
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

    GIVEN("an floating-point vector which contains elements with a value=2019.0") {
        etl::vector<double> v1(5, 2019.0);

        REQUIRE(5 == v1.size());
        REQUIRE(5 == v1.capacity());

        AND_GIVEN("a vector constructed from some another vector") {
            etl::vector<double> v2(v1);
            WHEN("the original vector was copied") {
                AND_WHEN("a new vector constructed with the contents of other") {
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

    GIVEN("an floating-point vector which contains elements with a value=2019.0") {
        etl::vector<double> v1(10, 2019.0);

        REQUIRE(10 == v1.size());
        REQUIRE(10 == v1.capacity());

        AND_GIVEN("a vector constructed from some another vector using move semantics") {
            etl::vector<double> v2(std::move(v1));
            WHEN("the original vector was moved") {
                AND_WHEN("a new vector constructed with the contents of other") {
                    THEN("its size and capacity must be equal to the size and capacity of the original vector") {
                        REQUIRE(10 == v2.size());
                        REQUIRE(10 == v2.capacity());
                        AND_THEN("its value of elements must be equal to 2019.0") {
                            for (auto& i : v2)
                                REQUIRE(2019.0 == i);
                        }
                    }
                    THEN("the original vector must be empty") { REQUIRE(v1.empty()); }
                }
            }
        }
    }
}

SCENARIO("vector capacity can be increased and decreased", "[vector]") {

    GIVEN("an empty vector of integer values") {
        etl::vector<int, UT_allocator<int>> v;
        WHEN("a vector capacity size was increased to a new value") {
            v.reserve(10);
            THEN("since new value is greater than the current capacity, new storage is allocated") {
                REQUIRE(0 == v.size());
                REQUIRE(10 == v.capacity());
            }
        }
    }

    GIVEN("an integer vector which already contains elements") {
        etl::vector<int, UT_allocator<int>> v(10);
        WHEN("a vector capacity size was increased to a new value") {
            v.reserve(20);
            THEN("since new value is greater than the current capacity, new storage is allocated") {
                REQUIRE(10 == v.size());
                REQUIRE(20 == v.capacity());
            }
        }
    }

    GIVEN("an integer vector which already contains elements") {
        etl::vector<int, UT_allocator<int>> v(20);
        WHEN("a vector capacity size was decreased to a new value") {
            v.reserve(10);
            THEN("since new value is lesser than the current capacity than the method does nothing") {
                REQUIRE(20 == v.size());
                REQUIRE(20 == v.capacity());
            }
        }
    }
}

SCENARIO("resizing the vector to contain count elements", "[vector]") {

    GIVEN("an empty vector of integer values") {
        etl::vector<int> v;
        WHEN("a vector is resized to a new count of elements") {
            v.resize(10);
            THEN("since size is less than new count, additional elements must be appended") {
                REQUIRE(10 == v.size());
                REQUIRE(10 == v.capacity());
            }
        }
    }

    GIVEN("an integer vector which already contains elements") {
        etl::vector<int> v(5);
        WHEN("a vector is resized to a new count of elements") {
            v.resize(10);
            THEN("since size is less than new count, additional elements must be appended") {
                REQUIRE(10 == v.size());
                REQUIRE(10 == v.capacity());
            }
        }
    }

    GIVEN("an integer vector which already contains elements with a value=2019.0") {
        etl::vector<int> v(5, 2019);
        WHEN("a vector is resized to a new count of elements with the value to initialize the new elements") {
            v.resize(10, 666);
            THEN("since size is less than new count, additional copies of value are appended") {
                REQUIRE(10 == v.size());
                REQUIRE(10 == v.capacity());
            }
        }
    }

    GIVEN("an integer vector which already contains elements with a value=2019.0") {
        etl::vector<int> v(10, 2019);
        WHEN("a vector is resized to a new count of elements") {
            v.resize(5);
            THEN("since size is greater than count, the vector is reduced to given count elements") {
                REQUIRE(5 == v.size());
                REQUIRE(10 == v.capacity());
            }
        }
        WHEN("a vector is resized to a new count of elements with the value to initialize the new elements") {
            v.resize(15, 666);
            THEN("since size is less than new count, additional copies of value are appended") {
                REQUIRE(15 == v.size());
                REQUIRE(15 == v.capacity());
            }
        }
    }
}
