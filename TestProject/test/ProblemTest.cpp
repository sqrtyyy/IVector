#include <iostream>
#include "tests.hpp"
#if defined _WIN32 || defined __CYGWIN__
  #include <windef.h>
  #include <winbase.h>
#elif __GNUC__ >= 4
  #include <dlfcn.h>
#endif

using BrockerGetter = void*(*)();

IProblem* ProblemTest::loadProblem() {
#if defined _WIN32 || defined __CYGWIN__
  HMODULE lib = LoadLibrary("../libs/problemImplSmirnova.dll");
#elif __GNUC__ >= 4
  void* lib = dlopen("../../libs//libAntonovProblem.so", RTLD_LAZY);
#endif
  if (lib == nullptr) {
    std::cout << "Unable to load library" << std::endl;
    return nullptr;
  }
#if defined _WIN32 || defined __CYGWIN__
  auto brokerGetterFun = (BrockerGetter)GetProcAddress(lib, "getBroker");
#elif __GNUC__ >= 4
  auto brokerGetterFun = (BrockerGetter)dlsym(lib, "getBroker");
#endif
  if (!brokerGetterFun) {
    std::cout << "Unable to load procedure" << std::endl;
#if defined _WIN32 || defined __CYGWIN__
    FreeLibrary(lib);
#elif __GNUC__ >= 4
    dlclose(lib);
#endif
    return nullptr;
  }
  auto* brokerInstance = (IBroker*)brokerGetterFun();
  return (IProblem*)brokerInstance->getInterfaceImpl(IBroker::INTERFACE_IMPL::IPROBLEM);
}

void ProblemTest::testCreate() {
  IProblem* problem = loadProblem();
  TEST_RESULT(problem)
  delete problem;
}

void ProblemTest::testParamsArgsDomain() {
  CREATE_LOGGER
  CREATE_VEC_THREE
  CREATE_VEC_SIX
  CREATE_VEC_ARG_ONE
  CREATE_VEC_ARG_TWO
  CREATE_MULTIDX_TWO
  CREATE_MULTIDX_ARG_ONE

  auto problem = loadProblem();
  auto compact = ICompact::createCompact(vec3, vec6, multIdx2);
  auto compact2 = ICompact::createCompact(vecArg1, vecArg2, multIdxDim2one);
  TEST_RESULT(problem->setParamsDomain(compact) == RC::SUCCESS)
  TEST_RESULT(problem->setParamsDomain(compact2) == RC::MISMATCHING_DIMENSIONS)
  TEST_RESULT(problem->setArgsDomain(compact2) == RC::SUCCESS)
  TEST_RESULT(problem->setArgsDomain(compact) == RC::MISMATCHING_DIMENSIONS)

  delete problem;
  delete compact;
  delete compact2;
  CLEAR_LOGGER
  CLEAR_VEC_SIX
  CLEAR_VEC_THREE
  CLEAR_VEC_ARG_ONE
  CLEAR_VEC_ARG_TWO
  CLEAR_MULTIDX_TWO
  CLEAR_MULTIDX_ARG_ONE
  PRINT_TEST_RESULT(true)
}

void ProblemTest::testClone() {
  CREATE_LOGGER
  CREATE_VEC_THREE
  CREATE_VEC_SIX
  CREATE_VEC_ARG_ONE
  CREATE_VEC_ARG_TWO
  CREATE_MULTIDX_TWO
  CREATE_MULTIDX_ARG_ONE

  auto problem = loadProblem();
  auto compact = ICompact::createCompact(vec3, vec6, multIdx2);
  auto compact2 = ICompact::createCompact(vecArg1, vecArg2, multIdxDim2one);
  TEST_RESULT(problem->setParamsDomain(compact) == RC::SUCCESS)
  TEST_RESULT(problem->setArgsDomain(compact2) == RC::SUCCESS)
  TEST_RESULT(problem->setArgs(vecArg1) == RC::SUCCESS)
  TEST_RESULT(problem->setParams(vec3) == RC::SUCCESS)

  auto problemCopy = problem->clone();
  TEST_RESULT(problemCopy->evalByArgs(vecArg1) == problem->evalByArgs(vecArg1))
  TEST_RESULT(problemCopy->evalByParams(vec6) == problem->evalByParams(vec6))

  delete compact;
  delete compact2;
  delete problem;
  delete problemCopy;
  CLEAR_LOGGER
  CLEAR_VEC_SIX
  CLEAR_VEC_THREE
  CLEAR_VEC_ARG_ONE
  CLEAR_VEC_ARG_TWO
  CLEAR_MULTIDX_TWO
  CLEAR_MULTIDX_ARG_ONE
  PRINT_TEST_RESULT(true)
}

void ProblemTest::testEvalByArgsByParams() {
  CREATE_LOGGER
  CREATE_VEC_THREE
  CREATE_VEC_SIX
  CREATE_VEC_ARG_ONE
  CREATE_VEC_ARG_TWO
  CREATE_MULTIDX_TWO
  CREATE_MULTIDX_ARG_ONE

  auto problem = loadProblem();
  auto compact = ICompact::createCompact(vec3, vec6, multIdx2);
  auto argsCompact = ICompact::createCompact(vecArg1, vecArg2, multIdxDim2one);
  problem->setParamsDomain(compact);
  problem->setArgsDomain(argsCompact);
  problem->setArgs(vecArg2);
  problem->setParams(vec6);

  TEST_RESULT(problem->evalByArgs(vecArg1) == 0)
  TEST_RESULT(problem->evalByArgs(vecArg2) == 390472)
  TEST_RESULT(problem->evalByParams(vec3) == 6240001)

  delete problem;
  delete compact;
  delete argsCompact;

  CLEAR_LOGGER
  CLEAR_VEC_SIX
  CLEAR_VEC_THREE
  CLEAR_VEC_ARG_ONE
  CLEAR_VEC_ARG_TWO
  CLEAR_MULTIDX_TWO
  CLEAR_MULTIDX_ARG_ONE
  PRINT_TEST_RESULT(true)
}

void ProblemTest::testEvalDerivativeByArgsByParams() {
  CREATE_LOGGER
  CREATE_VEC_THREE
  CREATE_VEC_SIX
  CREATE_VEC_ARG_ONE
  CREATE_VEC_ARG_TWO
  CREATE_MULTIDX_TWO
  CREATE_MULTIDX_ARG_ONE
  CREATE_DXDY_MULTIDX
  CREATE_DX_MULTIDX

  IDiffProblem* problem = dynamic_cast<IDiffProblem*>(loadProblem());
  auto compact = ICompact::createCompact(vec3, vec6, multIdx2);
  auto argsCompact = ICompact::createCompact(vecArg1, vecArg2, multIdxDim2one);
  TEST_RESULT(problem->setParamsDomain(compact) == RC::SUCCESS)
  TEST_RESULT(problem->setArgsDomain(argsCompact) == RC::SUCCESS)
  TEST_RESULT(problem->setArgs(vecArg2) == RC::SUCCESS)
  TEST_RESULT(problem->setParams(vec6) == RC::SUCCESS)

  TEST_RESULT(problem->evalDerivativeByArgs(vecArg1, multIdxDim2dxdy) == 0)
  TEST_RESULT(problem->evalDerivativeByParams(vec3, multIdxDim2dxdy) == 0)

  multIdxDim2dxdy->setAxisIndex(0, 0);
  TEST_RESULT(problem->evalDerivativeByArgs(vecArg2, multIdxDim2dxdy) == 312500)
  TEST_RESULT(problem->evalDerivativeByParams(vec3, multIdxDim2dxdy) == 5000000)

  multIdxDim2dxdy->setAxisIndex(1, 2);
  TEST_RESULT(problem->evalDerivativeByParams(vec3, multIdxDim2dxdy) == 3000000)

  TEST_RESULT(problem->evalDerivativeByArgs(vecArg1, multIdxDim2dx) == 0)
  TEST_RESULT(problem->evalDerivativeByArgs(vecArg2, multIdxDim2dx) == -274)
  TEST_RESULT(problem->evalDerivativeByParams(vec3, multIdxDim2dx) == -19996)

  delete problem;
  delete compact;
  delete argsCompact;
  CLEAR_LOGGER
  CLEAR_VEC_SIX
  CLEAR_VEC_THREE
  CLEAR_VEC_ARG_ONE
  CLEAR_VEC_ARG_TWO
  CLEAR_MULTIDX_TWO
  CLEAR_MULTIDX_ARG_ONE
  CLEAR_DXDY_MULTIDX
  CLEAR_DX_MULTIDX
  PRINT_TEST_RESULT(true)
}

void ProblemTest::testEvalGradientByArgsByParams() {
  CREATE_LOGGER
  CREATE_VEC_THREE
  CREATE_VEC_SIX
  CREATE_VEC_ARG_ONE
  CREATE_VEC_ARG_TWO
  CREATE_MULTIDX_TWO
  CREATE_MULTIDX_ARG_ONE
  auto* problem = dynamic_cast<IDiffProblem*>(loadProblem());
  auto compact = ICompact::createCompact(vec3, vec6, multIdx2);
  auto argsCompact = ICompact::createCompact(vecArg1, vecArg2, multIdxDim2one);
  problem->setParamsDomain(compact);
  problem->setArgsDomain(argsCompact);
  problem->setArgs(vecArg2);
  problem->setParams(vec6);

  IVector* vector1 = vecArg1->clone();
  IVector* vector2 = vecArg1->clone();
  TEST_RESULT(problem->evalGradientByParams(vec3, vector1) == RC::SUCCESS)
  TEST_RESULT(problem->evalGradientByArgs(vecArg2, vector2) == RC::SUCCESS)
  TEST_RESULT(vector1)
  TEST_RESULT(vector2)
  double const* vecData1 = vector1->getData();
  double const* vecData2 = vector2->getData();
  double result1[2] = {-19996, 5000000};
  double result2[2] = {-274, 312500};
  for (size_t i = 0; i < vector1->getDim(); ++i) {
    TEST_RESULT(vecData1[i] == result1[i])
    TEST_RESULT(vecData2[i] == result2[i])
  }
  delete vector1;
  delete vector2;
  delete problem;
  delete compact;
  delete argsCompact;
  CLEAR_LOGGER
  CLEAR_VEC_SIX
  CLEAR_VEC_THREE
  CLEAR_VEC_ARG_ONE
  CLEAR_VEC_ARG_TWO
  CLEAR_MULTIDX_TWO
  CLEAR_MULTIDX_ARG_ONE
  PRINT_TEST_RESULT(true)
}

void ProblemTest::testAll()
{
  std::cout << "Running all Problem tests" << std::endl;
  testCreate();
  testParamsArgsDomain();
  testClone();
  testEvalByArgsByParams();

  // for IDiffProblem
  testEvalDerivativeByArgsByParams();
  testEvalGradientByArgsByParams();

  std::cout << "Successfully ran all Problem tests" << std::endl;
}