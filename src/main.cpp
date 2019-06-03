
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <fstream>

#include "../include/set_bits.hpp"

#include <cstdlib>

#include <memory>

#include "../include/vector.hpp"

int main() {
    etl::vector<int> v1(3, 10);
    v1.push_back(5);
    v1.push_back(1);
    v1.push_back(3);

    std::cout << v1[3] << ' ' << v1[4];
    etl::vector<int> v2(v1);

    std::vector<int>::const_iterator it;

    // std::size_t t;
}
