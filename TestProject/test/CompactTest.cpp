#include <iostream>
#include <cstring>
#include <memory>
#include "tests.hpp"


void MultiIndexTest::testCreate()
{
  CREATE_LOGGER
  CREATE_MULTIDX_ONE

  TEST_RESULT(multIdx1 != nullptr);

  IMultiIndex* multiIndex = IMultiIndex::createMultiIndex(3, nullptr);

  TEST_RESULT(multiIndex == nullptr);

  multiIndex = IMultiIndex::createMultiIndex(0, data1_mlt.data());

  TEST_RESULT(multiIndex == nullptr);

  CLEAR_MULTIDX_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void MultiIndexTest::testGetDim()
{
  CREATE_LOGGER
  CREATE_MULTIDX_ONE
  CREATE_MULTIDX_TWO
  CREATE_MULTIDX_OTHER_DIM

  TEST_RESULT(multIdx1->getDim() == multIdx2->getDim());
  TEST_RESULT(multIdx1->getDim() == 3);
  TEST_RESULT(multIdx1->getDim() != multIdxOtherDim->getDim());


  CLEAR_MULTIDX_ONE
  CLEAR_MULTIDX_TWO
  CLEAR_MULTIDX_OTHER_DIM

  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void MultiIndexTest::testGetData()
{
  CREATE_LOGGER
  CREATE_MULTIDX_ONE

  const size_t* multIdx1Data = multIdx1->getData();

  TEST_RESULT(multIdx1Data != nullptr);
  TEST_RESULT(!std::memcmp(multIdx1Data, data1_mlt.data(), data1_mlt.size() * sizeof(size_t)))

  CLEAR_MULTIDX_ONE

  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void MultiIndexTest::testGetAxisIndex()
{
  CREATE_LOGGER
  CREATE_MULTIDX_ONE

  size_t val;
  TEST_RESULT(multIdx1->getAxisIndex(0, val) == RC::SUCCESS);
  TEST_RESULT(val == multIdx1->getData()[0]);

  TEST_RESULT(multIdx1->getAxisIndex(multIdx1->getDim(), val) == RC::INDEX_OUT_OF_BOUND);

  CLEAR_MULTIDX_ONE

  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void MultiIndexTest::testIncAxisIndex()
{
  CREATE_LOGGER
  CREATE_MULTIDX_ONE

  size_t val;
  multIdx1->getAxisIndex(0, val);

  TEST_RESULT(multIdx1->incAxisIndex(0, 1) == RC::SUCCESS)
  TEST_RESULT(val == multIdx1->getData()[0] - 1);

  TEST_RESULT(multIdx1->incAxisIndex(5, 1) == RC::INDEX_OUT_OF_BOUND)


  CLEAR_MULTIDX_ONE

  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void MultiIndexTest::testSetAxisIndex()
{
  CREATE_LOGGER
  CREATE_MULTIDX_ONE

  size_t val = 20;
  TEST_RESULT(val != multIdx1->getData()[0]);
  TEST_RESULT(multIdx1->setAxisIndex(0, val) == RC::SUCCESS)
  TEST_RESULT(val == multIdx1->getData()[0]);

  TEST_RESULT(multIdx1->setAxisIndex(5, 1) == RC::INDEX_OUT_OF_BOUND)


  CLEAR_MULTIDX_ONE

  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void MultiIndexTest::testSetData()
{
  CREATE_LOGGER
  CREATE_MULTIDX_ONE

  size_t indices[] = {2, 3, 4};

  TEST_RESULT(multIdx1->setData(3, indices) == RC::SUCCESS);
  TEST_RESULT(!std::memcmp(multIdx1->getData(), indices, sizeof(indices)))
  TEST_RESULT(multIdx1->setData(sizeof(indices) / sizeof(size_t), nullptr) == RC::NULLPTR_ERROR);
  TEST_RESULT(multIdx1->setData(0, indices) == RC::MISMATCHING_DIMENSIONS);

  CLEAR_MULTIDX_ONE

  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void MultiIndexTest::testClone()
{
  CREATE_LOGGER
  CREATE_MULTIDX_ONE

  IMultiIndex* multiIndex = multIdx1->clone();

  TEST_RESULT(multiIndex != nullptr);
  TEST_RESULT(!std::memcmp(multIdx1->getData(),  multiIndex->getData(), multiIndex->getDim() * sizeof(size_t)))
  size_t indices[] = {2, 3, 4, 6, 8};
  multIdx1->setData(sizeof(indices) / sizeof(size_t), indices);
  TEST_RESULT(!std::memcmp(multIdx1->getData(), multIdx1->getData(), sizeof(indices)))

  CLEAR_MULTIDX_ONE

  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void MultiIndexTest::testAll()
{
  std::cout << "Running all MultiIndex tests" << std::endl;

  testCreate();

  testGetDim();
  testGetData();
  testGetAxisIndex();
  testIncAxisIndex();
  testSetAxisIndex();
  testSetData();
  testClone();

  std::cout << "Successfully ran all MultiIndex tests" << std::endl;
}


void CompactTest::testCreate()
{
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_THREE
  CREATE_MULTIDX_ONE
  CREATE_MULTIDX_OTHER_DIM

  auto* compact = ICompact::createCompact(vec1, vec2, multIdxOtherDim);
  TEST_RESULT(compact != nullptr)
  delete compact;

  compact = ICompact::createCompact(vec1, vec1, multIdxOtherDim);
  TEST_RESULT(compact != nullptr)
  delete compact;

  compact = ICompact::createCompact(vec1, vec2, multIdx1);
  TEST_RESULT(compact == nullptr)

  compact = ICompact::createCompact(vec1, vec3, multIdxOtherDim);
  TEST_RESULT(compact == nullptr)

  compact = ICompact::createCompact(vec1, nullptr, multIdxOtherDim);
  TEST_RESULT(compact == nullptr)

  compact = ICompact::createCompact(nullptr, vec3, multIdxOtherDim);
  TEST_RESULT(compact == nullptr)

  compact = ICompact::createCompact(vec1, vec2, nullptr);
  TEST_RESULT(compact == nullptr)

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_VEC_THREE
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_MULTIDX_ONE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}


void CompactTest::testGetDim()
{
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_MULTIDX_OTHER_DIM

  auto* compact = ICompact::createCompact(vec1, vec2, multIdxOtherDim);
  TEST_RESULT(compact->getDim() == vec1->getDim());

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void CompactTest::testGetGrid()
{
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_MULTIDX_OTHER_DIM

  auto* compact = ICompact::createCompact(vec1, vec2, multIdxOtherDim);
  IMultiIndex* grid = compact->getGrid();
  TEST_RESULT(grid != nullptr);
  TEST_RESULT(grid->getDim() == multIdxOtherDim->getDim())
  TEST_RESULT(!std::memcmp(grid->getData(), multIdxOtherDim->getData(), grid->getDim() * sizeof(size_t)))

  delete compact;
  delete grid;

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void CompactTest::testGetLeftBoundary()
{
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_MULTIDX_OTHER_DIM

  auto* compact = ICompact::createCompact(vec1, vec2, multIdxOtherDim);
  IVector* val;
  TEST_RESULT(compact->getLeftBoundary(val) == RC::SUCCESS);
  for (unsigned i = 0; i < val->getDim(); ++i) {
    TEST_RESULT(val->getData()[i] == std::min(vec1->getData()[i], vec2->getData()[i]))
  }
  delete compact;
  delete val;

  compact = ICompact::createCompact(vec2, vec1, multIdxOtherDim);
  TEST_RESULT(compact->getLeftBoundary(val) == RC::SUCCESS);
  for (unsigned i = 0; i < val->getDim(); ++i) {
    TEST_RESULT(val->getData()[i] == std::min(vec1->getData()[i], vec2->getData()[i]))
  }
  delete compact;
  delete val;

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void CompactTest::testGetRightBoundary()
{
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_MULTIDX_OTHER_DIM

  auto* compact = ICompact::createCompact(vec1, vec2, multIdxOtherDim);
  IVector* val;
  TEST_RESULT(compact->getRightBoundary(val) == RC::SUCCESS);
  for (unsigned i = 0; i < val->getDim(); ++i) {
    TEST_RESULT(val->getData()[i] == std::max(vec1->getData()[i], vec2->getData()[i]))
  }
  delete compact;
  delete val;

  compact = ICompact::createCompact(vec2, vec1, multIdxOtherDim);
  TEST_RESULT(compact->getRightBoundary(val) == RC::SUCCESS);
  for (unsigned i = 0; i < val->getDim(); ++i) {
    TEST_RESULT(val->getData()[i] == std::max(vec1->getData()[i], vec2->getData()[i]))
  }
  delete compact;
  delete val;

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void CompactTest::testGetVectorCoords() {
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_MULTIDX_OTHER_DIM
  CREATE_MULTIDX_DIM4_TWO
  CREATE_MULTIDX_DIM4_THREE

  auto compact = ICompact::createCompact(vec1, vec2, multIdxOtherDim);
  IVector* vec = nullptr;
  TEST_RESULT(compact->getVectorCoords(multIdxDim4two,vec) == RC::NULLPTR_ERROR)
  TEST_RESULT(compact->getVectorCoords(multIdxDim4two, vec1) == RC::SUCCESS)
  std::array<double, 4> answer({3, 3.3125, 6, 7.75});
  double const* result = vec1->getData();

  for (size_t i = 0; i < data1.size(); ++i) {
    TEST_RESULT(result[i] == answer[i])
  }

  TEST_RESULT(compact->getVectorCoords(multIdxDim4three, vec1) == RC::INDEX_OUT_OF_BOUND)
  delete compact;
  delete vec;

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_MULTIDX_DIM4_TWO
  CLEAR_MULTIDX_DIM4_THREE
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void CompactTest::testGetVectorCopy() {
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_MULTIDX_OTHER_DIM
  CREATE_MULTIDX_DIM4_TWO
  auto compact = ICompact::createCompact(vec1, vec2, multIdxOtherDim);
  IVector* vec = nullptr;
  TEST_RESULT(compact->getVectorCopy(multIdxDim4two,vec) == RC::SUCCESS)
  std::array<double, 4> answer({3, 3.3125, 6, 7.75});
  double const* result = vec->getData();

  for (size_t i = 0; i < data1.size(); ++i) {
    TEST_RESULT(result[i] == answer[i])
  }
  delete compact;
  delete vec;

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_MULTIDX_DIM4_TWO
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void CompactTest::testIsInside() {
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_FIVE
  CREATE_VEC_FOUR
  CREATE_MULTIDX_OTHER_DIM

  auto compact = ICompact::createCompact(vec1, vec4, multIdxOtherDim);
  TEST_RESULT(compact->isInside(vec2) == false)
  TEST_RESULT(compact->isInside(vec5) == true)
  delete compact;

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_VEC_FIVE
  CLEAR_VEC_FOUR
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_LOGGER
}

void CompactTest::testClone() {
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_MULTIDX_OTHER_DIM
  CREATE_MULTIDX_DIM4_TWO

  auto compact = ICompact::createCompact(vec1, vec2, multIdxOtherDim);
  std::unique_ptr<ICompact> pCompart = std::unique_ptr<ICompact>(compact->clone());
  IVector* vec3 = nullptr;
  IVector* vec4 = nullptr;
  compact->getVectorCopy(multIdxDim4two ,vec3);
  pCompart->getVectorCopy(multIdxDim4two ,vec4);

  for (size_t idx = 0; idx < data1.size(); ++idx)
    TEST_RESULT(vec3->getData()[idx] == vec4->getData()[idx])

  delete compact;
  delete vec3;
  delete vec4;
  CLEAR_LOGGER
  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_MULTIDX_DIM4_TWO
  PRINT_TEST_RESULT(true)
}

void CompactTest::testIntersection() {
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_FIVE
  CREATE_VEC_FOUR
  CREATE_MULTIDX_OTHER_DIM

  auto compact1 = ICompact::createCompact(vec5, vec2, multIdxOtherDim);
  auto compact2 = ICompact::createCompact(vec1, vec4, multIdxOtherDim);
  auto intersection = ICompact::createIntersection(compact1, compact2, multIdxOtherDim, TOLERANCE);

  TEST_RESULT(intersection)
  IVector *left = nullptr, *right = nullptr;
  intersection->getLeftBoundary(left);
  intersection->getRightBoundary(right);
  TEST_RESULT(left && right)
  double const* responseLeft = left->getData();
  double const* responseRight = right->getData();
  double answerLeft[4] = {5, 5.5, 3, 7};
  double answerRight[4] = {15, 6, 6, 8.5};
  for (size_t i = 0; i < 4; ++i) {
    TEST_RESULT(responseLeft[i] == answerLeft[i])
    TEST_RESULT(responseRight[i] == answerRight[i])
  }

  delete(compact1);
  delete(compact2);
  delete(intersection);
  delete(left);
  delete(right);

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_VEC_FIVE
  CLEAR_VEC_FOUR
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void CompactTest::testSpan() {
  CREATE_LOGGER
  CREATE_VEC_ONE
  CREATE_VEC_TWO
  CREATE_VEC_FIVE
  CREATE_VEC_FOUR
  CREATE_MULTIDX_OTHER_DIM

  auto compact1 = ICompact::createCompact(vec5, vec2, multIdxOtherDim);
  auto compact2 = ICompact::createCompact(vec1, vec4, multIdxOtherDim);
  auto span = ICompact::createCompactSpan(compact1, compact2, multIdxOtherDim);

  TEST_RESULT(span)
  IVector *left = nullptr, *right = nullptr;
  span->getLeftBoundary(left);
  span->getRightBoundary(right);
  TEST_RESULT(left && right)
  double const* responseLeft = left->getData();
  double const* responseRight = right->getData();
  double answerLeft[4] = {1, 3, 3, 6};
  double answerRight[4] = {36, 8, 9, 8.5};
  for (size_t i = 0; i < 4; ++i) {
    double a = responseLeft[i], b = responseRight[i];
    TEST_RESULT(responseLeft[i] == answerLeft[i])
    TEST_RESULT(responseRight[i] == answerRight[i])
  }

  delete(compact1);
  delete(compact2);
  delete(span);
  delete(left);
  delete(right);

  CLEAR_VEC_ONE
  CLEAR_VEC_TWO
  CLEAR_VEC_FIVE
  CLEAR_VEC_FOUR
  CLEAR_MULTIDX_OTHER_DIM
  CLEAR_LOGGER
  PRINT_TEST_RESULT(true)
}

void CompactTest::testAll()
{
  std::cout << "Running all Compact tests" << std::endl;

  testCreate();

  testGetDim();
  testGetGrid();
  testGetLeftBoundary();
  testGetRightBoundary();
  testGetVectorCoords();
  testGetVectorCopy();
  testIsInside();
  testClone();
  testIntersection();
  testSpan();

  std::cout << "Successfully ran all Compact tests" << std::endl;
}