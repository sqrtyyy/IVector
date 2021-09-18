#pragma once

#include "../include/IVector.h"
#include "../include/ILogger.h"
#include "../include/ISet.h"
#include "../include/ICompact.h"
#include "../include/IMultiIndex.h"
#include "../include/IProblem.h"
#include "../include/IDiffProblem.h"
#include "../include/IBroker.h"
#include "../include/ISolver.h"
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
#define CREATE_VEC_FIVE std::array<double, 4> data5({15, 6, 3, 8.5}); IVector* vec5 = IVector::createVector(data5.size(), data5.data());
#define CREATE_VEC_THREE std::array<double, 3> data3({1, 10, 100}); IVector* vec3 = IVector::createVector(data3.size(), data3.data());
#define CREATE_VEC_SIX std::array<double, 3> data6({2, 5, 13}); IVector* vec6 = IVector::createVector(data6.size(), data6.data());
#define CREATE_VEC_ARG_ONE std::array<double, 2> dataArg1({0, 0}); IVector* vecArg1 = IVector::createVector(dataArg1.size(), dataArg1.data());
#define CREATE_VEC_ARG_TWO std::array<double, 2> dataArg2({1, 5}); IVector* vecArg2 = IVector::createVector(dataArg2.size(), dataArg2.data());
#define CREATE_VEC_ARG_THREE std::array<double, 2> dataArg3({2, 0.002}); IVector* vecArg3 = IVector::createVector(dataArg3.size(), dataArg3.data());
#define CREATE_VEC_SLV_PARAM1 std::array<double, 2> dataPrm1({0.0005, 0.5}); IVector* vecPrm1 = IVector::createVector(dataPrm1.size(), dataPrm1.data());
#define CREATE_VEC_SLV_PARAM2 std::array<double, 2> dataPrm2({0.0005, 2}); IVector* vecPrm2 = IVector::createVector(dataPrm2.size(), dataPrm2.data());
#define CREATE_VEC_SLV_PARAM3 std::array<double, 2> dataPrm3({0.5, 0.7}); IVector* vecPrm3 = IVector::createVector(dataPrm3.size(), dataPrm3.data());


#define CREATE_MULTIDX_ONE std::array<size_t, 3> data1_mlt({3, 5, 10}); IMultiIndex* multIdx1 = IMultiIndex::createMultiIndex(data1_mlt.size(), data1_mlt.data());
#define CREATE_MULTIDX_TWO std::array<size_t, 3> data2_mlt({1, 4, 7}); IMultiIndex* multIdx2 = IMultiIndex::createMultiIndex(data2_mlt.size(), data2_mlt.data());
#define CREATE_MULTIDX_THREE std::array<size_t, 3> data3_mlt({2, 8, 1}); IMultiIndex* multIdx3 = IMultiIndex::createMultiIndex(data3_mlt.size(), data3_mlt.data());
#define CREATE_MULTIDX_OTHER_DIM std::array<size_t, 4> data4_mlt({2, 8, 1, 4}); IMultiIndex* multIdxOtherDim = IMultiIndex::createMultiIndex(data4_mlt.size(), data4_mlt.data());
#define CREATE_MULTIDX_DIM4_TWO std::array<size_t, 4> data5_mlt({1, 1, 0, 2}); IMultiIndex* multIdxDim4two = IMultiIndex::createMultiIndex(data5_mlt.size(), data5_mlt.data());
#define CREATE_MULTIDX_DIM4_THREE std::array<size_t, 4> data6_mlt({3, 8, 5, 3}); IMultiIndex* multIdxDim4three = IMultiIndex::createMultiIndex(data6_mlt.size(), data6_mlt.data());
#define CREATE_MULTIDX_ARG_ONE std::array<size_t, 2> data_arg_mlt({10, 10}); IMultiIndex* multIdxDim2one = IMultiIndex::createMultiIndex(data_arg_mlt.size(), data_arg_mlt.data());
#define CREATE_DXDY_MULTIDX std::array<size_t, 2> data_dxdy_mlt({1, 1}); IMultiIndex* multIdxDim2dxdy = IMultiIndex::createMultiIndex(data_dxdy_mlt.size(), data_dxdy_mlt.data());
#define CREATE_DX_MULTIDX std::array<size_t, 2> data_dx_mlt({1, 0}); IMultiIndex* multIdxDim2dx = IMultiIndex::createMultiIndex(data_dx_mlt.size(), data_dx_mlt.data());

#define CLEAR_VEC_ONE delete vec1;
#define CLEAR_VEC_TWO delete vec2;
#define CLEAR_VEC_FOUR delete vec4;
#define CLEAR_VEC_THREE delete vec3;
#define CLEAR_VEC_FIVE delete vec5;
#define CLEAR_VEC_SIX delete vec6;
#define CLEAR_VEC_ARG_ONE delete vecArg1;
#define CLEAR_VEC_ARG_TWO delete vecArg2;
#define CLEAR_VEC_ARG_THREE delete vecArg3;
#define CLEAR_VEC_SLV_PARAM1 delete vecPrm1;
#define CLEAR_VEC_SLV_PARAM2 delete vecPrm2;
#define CLEAR_VEC_SLV_PARAM3 delete vecPrm3;
#define CLEAR_SET_ONE delete set1;
#define CLEAR_SET_TWO delete set2;
#define CLEAR_MULTIDX_ONE delete multIdx1;
#define CLEAR_MULTIDX_TWO delete multIdx2;
#define CLEAR_MULTIDX_THREE delete multIdx3;
#define CLEAR_MULTIDX_OTHER_DIM delete multIdxOtherDim;
#define CLEAR_MULTIDX_DIM4_TWO delete multIdxDim4two;
#define CLEAR_MULTIDX_DIM4_THREE delete multIdxDim4three;
#define CLEAR_MULTIDX_ARG_ONE delete multIdxDim2one;
#define CLEAR_DXDY_MULTIDX delete multIdxDim2dxdy;
#define CLEAR_DX_MULTIDX delete multIdxDim2dx;
#define CLEAR_LOGGER delete logger;

#define RESULT_STRING(isSuccess) isSuccess ? "Success" : "Failure"
#define PRINT_TEST_RESULT(isSuccess) printf("%s : ", __func__);        \
                                     printf(RESULT_STRING(isSuccess)); \
                                     printf(" %i\n", __LINE__);
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
  void testIntersection();
  void testSpan();

  void testAll();
}

namespace ProblemTest {
  IProblem* loadProblem();
  void testCreate();
  void testClone();
  void testParamsArgsDomain();
  void testEvalByArgsByParams();
  void testEvalDerivativeByArgsByParams();
  void testEvalGradientByArgsByParams();
  void testAll();
}

namespace SolverTest {
  void testCreate();
  void testClone();
  void testParamsDomain();
  void testArgsDomain();
  void testSolve();
  void testAll();
}