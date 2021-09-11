#include <array>
#include <memory>
#include <iostream>
#include <cmath>
#include "tests.hpp"


void VecTest::testCreate() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    TEST_RESULT(vec1 != nullptr);

    IVector* tmp = IVector::createVector(1, nullptr);
    TEST_RESULT(tmp == nullptr);

    delete tmp;
    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(1)
}

void VecTest::testCopy() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    if (IVector::copyInstance(vec2, vec1) != RC::SUCCESS)
        return;

    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(vec1->getData()[idx] == vec2->getData()[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
    PRINT_TEST_RESULT(true)
}
void VecTest::testMove() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    IVector::moveInstance(vec2, vec1);

    TEST_RESULT(vec1 == nullptr);
    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(data1[idx] == vec2->getData()[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
    PRINT_TEST_RESULT(true)
}

void VecTest::testClone() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    std::unique_ptr<IVector> vec2 = std::unique_ptr<IVector>(vec1->clone());

    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(vec1->getData()[idx] == vec2->getData()[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}
void VecTest::testGetData() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(vec1->getData()[idx] == data1[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}
void VecTest::testSetData() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    vec1->setData(data2.size(), data2.data());

    for (size_t idx = 0; idx < data2.size(); ++idx)
        TEST_RESULT(vec1->getData()[idx] == data2[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
    PRINT_TEST_RESULT(true)
}

void VecTest::testGetCord() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    double val;
    vec1->getCord(2, val);
    TEST_RESULT(val == 6);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}
void VecTest::testSetCord() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    vec1->setCord(1, 45);
    TEST_RESULT(vec1->getData()[1] == 45);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}
void VecTest::testScale() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    vec1->scale(2);
    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(vec1->getData()[idx] == data1[idx] * 2);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}
void VecTest::testGetDim() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    TEST_RESULT(vec1->getDim() == 4);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}

void VecTest::testInc() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    vec1->inc(vec2);
    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(vec1->getData()[idx] == data1[idx] + data2[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
    PRINT_TEST_RESULT(true)
}
void VecTest::testDec() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    vec1->dec(vec2);
    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(vec1->getData()[idx] == data1[idx] - data2[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
    PRINT_TEST_RESULT(true)
}

void VecTest::testAdd() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    IVector* vec3 = IVector::add(vec1, vec2);
    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(vec3->getData()[idx] == data1[idx] + data2[idx]);

    delete vec3;
    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
    PRINT_TEST_RESULT(true)
}
void VecTest::testSub() {
        CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    IVector* vec3 = IVector::sub(vec1, vec2);
    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(vec3->getData()[idx] == data1[idx] - data2[idx]);

    delete vec3;
    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
    PRINT_TEST_RESULT(true)
}

void VecTest::testDot() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    TEST_RESULT(IVector::dot(vec1, vec2) == 135);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
    PRINT_TEST_RESULT(true)
}
void VecTest::testEquals() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    IVector* vec2 = vec1->clone();
    TEST_RESULT(IVector::equals(vec1, vec2, IVector::NORM::FIRST, TOLERANCE));

    delete vec2;
    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}

void VecTest::testFirstNorm() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    TEST_RESULT(vec1->norm(IVector::NORM::FIRST) == 21);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}
void VecTest::testSecondNorm() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    TEST_RESULT(fabs(vec1->norm(IVector::NORM::SECOND) - 11.81101181) < TOLERANCE);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}
void VecTest::testChebyshevNorm() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    TEST_RESULT(vec1->norm(IVector::NORM::CHEBYSHEV) == 8.5);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}

void VecTest::testApplyFunc() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    vec1->applyFunction([](double val){ return val + 1; });
    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(vec1->getData()[idx] == data1[idx] + 1);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}
void VecTest::testForeach() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    vec1->foreach([](double val){ });
    for (size_t idx = 0; idx < data1.size(); ++idx)
        TEST_RESULT(vec1->getData()[idx] == data1[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    PRINT_TEST_RESULT(true)
}

void VecTest::testAll() {

  std::cout << "Running all Vector tests" << std::endl;
  testCreate();
  testCopy();
  testMove();
  testClone();
  testGetData();
  testSetData();
  testGetCord();
  testSetCord();
  testScale();
  testGetDim();
  testInc();
  testDec();
  testAdd();
  testSub();
  testDot();
  testEquals();
  testFirstNorm();
  testSecondNorm();
  testChebyshevNorm();
  testApplyFunc();
  testForeach();

    std::cout << "Successfully ran all Vector tests" << std::endl;
}