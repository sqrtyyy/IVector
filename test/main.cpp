#include "tests.hpp"

#include <iostream>

int main() {
    VecTest::testAll();
    std::cout << std::endl;
    SetTest::testAll();
    double x1[] = {0,1,2};
    double x2[] = {1,1,2};
    size_t indices[] = {1, 2, 3};
    IVector* vec1 = IVector::createVector(3, x1);
    IVector* vec2 = IVector::createVector(3, x2);
    IMultiIndex* order = IMultiIndex::createMultiIndex(3, indices);
    ICompact* compact = ICompact::createCompact(vec1, vec2, order);
    return 0;
}