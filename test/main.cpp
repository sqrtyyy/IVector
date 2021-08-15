#include "tests.hpp"

#include <iostream>

int main() {
    VecTest::testAll();
    std::cout << std::endl;
    SetTest::testAll();
    return 0;
}