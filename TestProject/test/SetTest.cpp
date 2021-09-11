#include <array>
#include <memory>
#include <iostream>
#include "tests.hpp"


void SetTest::testCreate() {
    CREATE_LOGGER
    CREATE_SET_ONE
    
    TEST_RESULT(set1 != nullptr);

    CLEAR_SET_ONE
    CLEAR_LOGGER
    PRINT_TEST_RESULT(true)
}


void SetTest::testFindFirst() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_THREE

  RC err;

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);

  err = set1->findFirst(vec1, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::SUCCESS)

  err = set1->findFirst(vec2, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::VECTOR_NOT_FOUND)

  err = set1->findFirst(nullptr, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::NULLPTR_ERROR)

  err = set1->findFirst(vec3, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::MISMATCHING_DIMENSIONS)

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_VEC_THREE
  CLEAR_SET_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testFindFirstAndCopy() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_VEC_ONE
  CREATE_VEC_TWO

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);

  IVector* vec = nullptr;
  RC err;

  err = set1->findFirstAndCopy(vec1, IVector::NORM::FIRST, TOLERANCE, vec);

  TEST_RESULT(IVector::equals(vec, vec1, IVector::NORM::FIRST, TOLERANCE))
  TEST_RESULT(err == RC::SUCCESS)

  err = set1->findFirstAndCopy(vec2, IVector::NORM::FIRST, TOLERANCE, vec);
  TEST_RESULT(vec == nullptr);
  TEST_RESULT(err == RC::VECTOR_NOT_FOUND);

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_SET_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testFindFirstAndCopyCoords() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_THREE

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);

  RC err;

  err = set1->findFirstAndCopyCoords(vec1, IVector::NORM::FIRST, TOLERANCE, vec2);

  TEST_RESULT(IVector::equals(vec2, vec1, IVector::NORM::FIRST, TOLERANCE))
  TEST_RESULT(err == RC::SUCCESS)

  err = set1->findFirstAndCopyCoords(vec1, IVector::NORM::FIRST, TOLERANCE, nullptr);

  TEST_RESULT(err == RC::NULLPTR_ERROR)

  err = set1->findFirstAndCopyCoords(vec2, IVector::NORM::FIRST, TOLERANCE, vec3);

  TEST_RESULT(err == RC::MISMATCHING_DIMENSIONS);

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_VEC_THREE
  CLEAR_SET_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void SetTest::testInsert() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_THREE

  RC err;

  err = set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::SUCCESS);
  TEST_RESULT(set1->getSize() == 1);
  TEST_RESULT(set1->getDim() == vec1->getDim());

  err = set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::SUCCESS);
  TEST_RESULT(set1->getSize() == 2);
  TEST_RESULT(set1->getDim() == vec1->getDim());

  err = set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::VECTOR_ALREADY_EXIST);
  TEST_RESULT(set1->getSize() == 2);
  TEST_RESULT(set1->getDim() == vec1->getDim());

  err = set1->insert(vec3, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::MISMATCHING_DIMENSIONS);
  TEST_RESULT(set1->getSize() == 2);
  TEST_RESULT(set1->getDim() == vec1->getDim());

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_VEC_THREE
  CLEAR_SET_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void SetTest::testGetDim() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_SET_TWO
  CREATE_VEC_ONE
  CREATE_VEC_THREE

  TEST_RESULT(set1->getDim() == 0);

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(vec1->getDim() == set1->getDim());

  set2->insert(vec3, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(set2->getDim() != set1->getDim());

  CLEAR_LOGGER
  CLEAR_SET_ONE
  CLEAR_SET_TWO
  CLEAR_VEC_ONE
  CLEAR_VEC_THREE
  PRINT_TEST_RESULT(true)
}
void SetTest::testGetSize() {
  CREATE_LOGGER
  CREATE_SET_ONE

  TEST_RESULT(set1->getSize() == 0);

  CLEAR_LOGGER
  CLEAR_SET_ONE
  PRINT_TEST_RESULT(true)
}
void SetTest::testGetCopy() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_VEC_ONE
  CREATE_VEC_TWO

  RC err;
  IVector* tmp;

  err = set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::SUCCESS);
  err = set1->getCopy(0, tmp);
  TEST_RESULT(err == RC::SUCCESS);
  TEST_RESULT(IVector::equals(vec1, tmp, IVector::NORM::FIRST, TOLERANCE));
  err = set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::SUCCESS);

  err = set1->getCopy(0, tmp);
  TEST_RESULT(err == RC::SUCCESS);
  TEST_RESULT(IVector::equals(vec1, tmp, IVector::NORM::FIRST, TOLERANCE));
  delete tmp;
  tmp = nullptr;
  err = set1->getCopy(1, tmp);
  TEST_RESULT(err == RC::SUCCESS);
  TEST_RESULT(IVector::equals(vec2, tmp, IVector::NORM::FIRST, TOLERANCE));
  delete tmp;

  CLEAR_VEC_TWO
  CLEAR_VEC_ONE
  CLEAR_SET_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testGetCoords() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_VEC_ONE
  CREATE_VEC_TWO

  RC err;

  err = set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::SUCCESS);

  err = set1->getCoords(0, vec2);
  TEST_RESULT(err == RC::SUCCESS);
  TEST_RESULT(IVector::equals(vec1, vec2, IVector::NORM::FIRST, TOLERANCE));

  CLEAR_VEC_TWO
  CLEAR_VEC_ONE
  CLEAR_SET_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testRemoveByIndex() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_VEC_ONE
  CREATE_VEC_TWO

  RC err;

  err = set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::SUCCESS);
  err = set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::SUCCESS);

  err = set1->remove(0);
  TEST_RESULT(err == RC::SUCCESS);
  TEST_RESULT(set1->getSize() == 1);
  TEST_RESULT(set1->findFirst(vec1, IVector::NORM::FIRST, TOLERANCE) == RC::VECTOR_NOT_FOUND);

  err = set1->remove(1);
  TEST_RESULT(err == RC::INDEX_OUT_OF_BOUND);
  TEST_RESULT(set1->getSize() == 1);

  err = set1->remove(0);
  TEST_RESULT(err == RC::SUCCESS);
  TEST_RESULT(set1->getSize() == 0);

  CLEAR_VEC_TWO
  CLEAR_VEC_ONE
  CLEAR_SET_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testRemoveByPattern() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_VEC_ONE
  CREATE_VEC_TWO

  RC err;

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);

  err = set1->remove(vec1, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(err == RC::SUCCESS);
  TEST_RESULT(set1->getSize() == 1);
  TEST_RESULT(set1->findFirst(vec1, IVector::NORM::FIRST, TOLERANCE) == RC::VECTOR_NOT_FOUND);

  set1->remove(vec1, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(set1->getSize() == 1);

  CLEAR_VEC_TWO
  CLEAR_VEC_ONE
  CLEAR_SET_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testEquals() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_SET_TWO
  CREATE_VEC_ONE
  CREATE_VEC_TWO

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(!ISet::equals(set1, set2, IVector::NORM::FIRST, TOLERANCE));
  set2->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(ISet::equals(set1, set2, IVector::NORM::FIRST, TOLERANCE));
  TEST_RESULT(set1->getSize() == set2->getSize());

  set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);
  TEST_RESULT(!ISet::equals(set1, set2, IVector::NORM::FIRST, TOLERANCE));

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_SET_ONE
  CLEAR_SET_TWO
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testClone() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_VEC_ONE
  CREATE_VEC_TWO

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);
  ISet* set2 = set1->clone();

  TEST_RESULT(ISet::equals(set1, set2, IVector::NORM::FIRST, TOLERANCE));

  delete set2;

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_SET_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testMakeIntersection() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_SET_TWO
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_FOUR

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);
  set2->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set2->insert(vec4, IVector::NORM::FIRST, TOLERANCE);

  ISet* set_result = ISet::makeIntersection(set1, set2, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(set_result != nullptr)
  TEST_RESULT(set_result->findFirst(vec1, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)
  TEST_RESULT(set_result->findFirst(vec2, IVector::NORM::FIRST, TOLERANCE) == RC::VECTOR_NOT_FOUND)
  TEST_RESULT(set_result->findFirst(vec4, IVector::NORM::FIRST, TOLERANCE) == RC::VECTOR_NOT_FOUND)

  delete set_result;

  set1->insert(vec4, IVector::NORM::FIRST, TOLERANCE);

  set_result = ISet::makeIntersection(set1, set2, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(set_result != nullptr)
  TEST_RESULT(set_result->findFirst(vec1, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)
  TEST_RESULT(set_result->findFirst(vec2, IVector::NORM::FIRST, TOLERANCE) == RC::VECTOR_NOT_FOUND)
  TEST_RESULT(set_result->findFirst(vec4, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)

  delete set_result;

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_SET_ONE
  CLEAR_SET_TWO
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)

}
void SetTest::testMakeUnion() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_SET_TWO
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_FOUR

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);
  set2->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set2->insert(vec4, IVector::NORM::FIRST, TOLERANCE);

  ISet* set_result = ISet::makeUnion(set1, set2, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(set_result->getSize() == 3)
  TEST_RESULT(set_result->findFirst(vec1, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)
  TEST_RESULT(set_result->findFirst(vec2, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)
  TEST_RESULT(set_result->findFirst(vec4, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)

  delete set_result;

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_SET_ONE
  CLEAR_SET_TWO
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testSub() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_SET_TWO
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_FOUR

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);
  set2->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set2->insert(vec4, IVector::NORM::FIRST, TOLERANCE);

  ISet* set_result = ISet::sub(set1, set2, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(set_result != nullptr)
  TEST_RESULT(set_result->getSize() == 1)
  TEST_RESULT(set_result->findFirst(vec2, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)
  TEST_RESULT(set_result->findFirst(vec1, IVector::NORM::FIRST, TOLERANCE) == RC::VECTOR_NOT_FOUND)

  delete set_result;

  set2->remove(vec1, IVector::NORM::FIRST, TOLERANCE);
  set2->insert(vec2, IVector::NORM::FIRST, TOLERANCE);

  set_result = ISet::sub(set1, set2, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(set_result != nullptr)
  TEST_RESULT(set_result->getSize() == 1)
  TEST_RESULT(set_result->findFirst(vec1, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)
  TEST_RESULT(set_result->findFirst(vec2, IVector::NORM::FIRST, TOLERANCE) == RC::VECTOR_NOT_FOUND)

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_VEC_FOUR
  CLEAR_SET_ONE
  CLEAR_SET_TWO
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testSymSub() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_SET_TWO
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_FOUR

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);

  set2->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set2->insert(vec4, IVector::NORM::FIRST, TOLERANCE);

  ISet* set_result = ISet::symSub(set1, set2, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(set_result != nullptr)
  TEST_RESULT(set_result->getSize() == 2)
  TEST_RESULT(set_result->findFirst(vec1, IVector::NORM::FIRST, TOLERANCE) == RC::VECTOR_NOT_FOUND)
  TEST_RESULT(set_result->findFirst(vec2, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)
  TEST_RESULT(set_result->findFirst(vec4, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)

  delete set_result;

  set2->remove(vec1, IVector::NORM::FIRST, TOLERANCE);
  set2->insert(vec2, IVector::NORM::FIRST, TOLERANCE);

  set_result = ISet::symSub(set1, set2, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(set_result != nullptr)
  TEST_RESULT(set_result->getSize() == 2)
  TEST_RESULT(set_result->findFirst(vec2, IVector::NORM::FIRST, TOLERANCE) == RC::VECTOR_NOT_FOUND)
  TEST_RESULT(set_result->findFirst(vec1, IVector::NORM::FIRST, TOLERANCE) == RC::SUCCESS)

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_VEC_FOUR
  CLEAR_SET_ONE
  CLEAR_SET_TWO
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}
void SetTest::testSubSet() {
  CREATE_LOGGER
  CREATE_SET_ONE
  CREATE_SET_TWO
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_FOUR

  set1->insert(vec1, IVector::NORM::FIRST, TOLERANCE);
  set1->insert(vec2, IVector::NORM::FIRST, TOLERANCE);

  set2->insert(vec1, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(ISet::subSet(set1, set2, IVector::NORM::FIRST, TOLERANCE))

  set2->insert(vec4, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(!ISet::subSet(set1, set2, IVector::NORM::FIRST, TOLERANCE))

  set1->insert(vec4, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(ISet::subSet(set1, set2, IVector::NORM::FIRST, TOLERANCE))

  set2->insert(vec2, IVector::NORM::FIRST, TOLERANCE);

  TEST_RESULT(ISet::subSet(set1, set2, IVector::NORM::FIRST, TOLERANCE))
  TEST_RESULT(ISet::subSet(set2, set1, IVector::NORM::FIRST, TOLERANCE))

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_VEC_FOUR
  CLEAR_SET_ONE
  CLEAR_SET_TWO
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void SetTest::testAll() {
    std::cout << "Running all Set tests" << std::endl;

    testCreate();
    testFindFirst();
    testFindFirstAndCopy();
    testFindFirstAndCopyCoords();
    testInsert();
    testGetDim();
    testGetSize();
    testGetCopy();
    testGetCoords();
    testRemoveByIndex();
    testRemoveByPattern();
    testEquals();
    testClone();
    testMakeIntersection();
    testMakeUnion();
    testSub();
    testSymSub();
    testSubSet();

    std::cout << "Successfully ran all Set tests" << std::endl;
}