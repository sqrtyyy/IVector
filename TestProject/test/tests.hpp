#pragma once

#include "IVector.h"
#include "ILogger.h"
#include "ISet.h"
#include "ICompact.h"
#include <cassert>
#include <cstdio>

#define TOLERANCE 1e-6

#define CREATE_LOGGER ILogger* logger = ILogger::createLogger(); \
IVector::setLogger(logger);                                      \
ISet::setLogger(logger);                                         \
ICompact::setLogger(logger);                                     \
IMultiIndex::setLogger(logger);
#define CREATE_SET_ONE ISet* set1 = ISet::createSet();
#define CREATE_SET_TWO ISet* set2 = ISet::createSet();
#define CREATE_VEC_ONE std::array<double, 4> data1({1, 5.5, 6, 8.5}); IVector* vec1 = IVector::createVector(data1.size(), data1.data());
#define CREATE_VEC_TWO std::array<double, 4> data2({5, 3, 9, 7}); IVector* vec2 = IVector::createVector(data2.size(), data2.data());
#define CREATE_VEC_FOUR std::array<double, 4> data4({36, 8, 3, 6}); IVector* vec4 = IVector::createVector(data4.size(), data4.data());
#define CREATE_VEC_THREE std::array<double, 3> data3({1, 10, 100}); IVector* vec3 = IVector::createVector(data3.size(), data3.data());



#define CREATE_MULTIDX_ONE std::array<size_t, 3> data1_mlt({3, 5, 10}); IMultiIndex* multIdx1 = IMultiIndex::createMultiIndex(3, data1_mlt.data());
#define CREATE_MULTIDX_TWO std::array<size_t, 3> data2_mlt({1, 4, 7}); IMultiIndex* multIdx2 = IMultiIndex::createMultiIndex(3, data2_mlt.data());
#define CREATE_MULTIDX_THREE std::array<size_t, 3> data3_mlt({2, 8, 1}); IMultiIndex* multIdx3 = IMultiIndex::createMultiIndex(3, data3_mlt.data());
#define CREATE_MULTIDX_OTHER_DIM std::array<size_t, 4> data4_mlt({2, 8, 1, 4}); IMultiIndex* multIdxOtherDim = IMultiIndex::createMultiIndex(4, data4_mlt.data());

#define CLEAR_VEC_ONE delete vec1;
#define CLEAR_VEC_TWO delete vec2;
#define CLEAR_VEC_FOUR delete vec4;
#define CLEAR_VEC_THREE delete vec3;
#define CLEAR_SET_ONE delete set1;
#define CLEAR_SET_TWO delete set2;
#define ClEAR_MULTIDX_ONE delete multIdx1;
#define ClEAR_MULTIDX_TWO delete multIdx2;
#define ClEAR_MULTIDX_THREE delete multIdx3;
#define ClEAR_MULTIDX_OTHER_DIM delete multIdxOtherDim;
#define CLEAR_LOGGER delete logger;

#define RESET   "\033[0m"
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */

#define RESULT_STRING(isSuccess) isSuccess ? BOLDGREEN "Success" RESET : BOLDRED "Failure" RESET
#define PRINT_TEST_RESULT(isSuccess) printf(BOLDBLACK "%s" RESET " : ", __func__); \
                                     printf(RESULT_STRING(isSuccess));          \
                                     printf(" %i", __LINE__);                                 \
                                     printf("\n");
#define TEST_RESULT(result)  if (!(result)){PRINT_TEST_RESULT(false); assert(false);}
namespace VecTest {
  void testCreate();

  void testCopy();
  void testMove();

  void testClone();
  void testGetData();
  void testSetData();

  void testGetCord();
  void testSetCord();
  void testScale();
  void testGetDim();

  void testInc();
  void testDec();

  void testAdd();
  void testSub();

  void testDot();
  void testEquals();

  void testFirstNorm();
  void testSecondNorm();
  void testChebyshevNorm();

  void testApplyFunc();
  void testForeach();
    
  void testAll();
}

namespace SetTest {
  void testCreate();

  void testGetDim();
  void testGetSize();
  void testGetCopy();
  void testGetCoords();

  void testInsert();
  void testRemoveByIndex();
  void testRemoveByPattern();

  void testClone();

  void testFindFirst();
  void testFindFirstAndCopy();
  void testFindFirstAndCopyCoords();

  void testMakeIntersection();
  void testMakeUnion();
  void testSub();
  void testSymSub();
  void testEquals();
  void testSubSet();

  void testAll();
};

namespace MultiIndexTest {
  void testCreate();

  void testGetDim();
  void testGetAxisIndex();
  void testGetData();
  void testIncAxisIndex();
  void testSetAxisIndex();
  void testSetData();
  void testClone();

  void testAll();
}

namespace CompactTest {
  void testCreate();

  void testGetDim();
  void testGetGrid();
  void testGetLeftBoundary();
  void testGetRightBoundary();
  void testIncAxisIndex();
  void testGetVectorCoords();
  void testGetVectorCopy();
  void testIsInside();
  void testClone();

  void testAll();
}