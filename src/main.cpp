#include <cstdlib>
#include <iostream>
#include <memory>

#include "../include/vector.hpp"

int main() {
    etl::vector<int> v1;

#if 1
    v1.push_back(5);
    v1.push_back(1);
    v1.push_back(3);
    v1.push_back(7);
    v1.insert(v1.begin() + 2, 666);

    v1.push_back(4);
    v1.insert(v1.begin(), 3, 777);

    for (std::size_t i = 0; i < v1.size(); ++i) {
        std::cout << v1[i] << ' ';
    }
    std::cout << std::endl;

    etl::vector<int> v2(2, 400);
    v1.reserve(16);
    v1.insert(v1.begin() + 2, v2.begin(), v2.end());

    int arr[]{501, 502, 503};
    v1.insert(v1.begin(), arr, arr + 3);

    for (std::size_t i = 0; i < v1.size(); ++i) {
        std::cout << v1[i] << ' ';
    }
    std::cout << std::endl;
#endif
}
