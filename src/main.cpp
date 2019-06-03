
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
    v1.push_back(1); /*
    v1.push_back(3);
     for (std::size_t i = 0; i < v1.size(); ++i) {
         std::cout << v1[i] << ' ';
     }
     std::cout << std::endl;*/
    /*
        etl::vector<int> v2(v1);
        for (std::size_t i = 0; i < v2.size(); ++i) {
            std::cout << v2[i] << ' ';
        }
        std::cout << std::endl;*/

    std::vector<int>::const_iterator it;

    // std::size_t t;
}
