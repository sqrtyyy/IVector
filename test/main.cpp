#include "tests.hpp"

#include <iostream>

int main() {
    VecTest::testAll();
    std::cout << std::endl;
    SetTest::testAll();
    std::cout << std::endl;
    MultiIndexTest::testAll();
    std::cout << std::endl;
    CompactTest::testAll();
    return 0;
}