//
// Created by aleksei on 10.09.2021.
//

#include "Solver.h"

#include "../Helpers/Helpers.h"

const unsigned Solver::m_methodParamsNum = 1;
const unsigned Solver::m_maxStepsNum = 100;

Solver::Solver()
: m_problem(nullptr)
, m_args(nullptr)
, m_params(nullptr)
, m_solution(nullptr)
{}
RC Solver::calcHessian(IVector const* const x, double * const* const H) const
{
  size_t dataMultiIndex[2] = {0};
  dataMultiIndex[0] = 2;
  dataMultiIndex[1] = 0;
  IMultiIndex* derDegrees = IMultiIndex::createMultiIndex(2, dataMultiIndex);
  if (derDegrees == nullptr) {
    return RC::ALLOCATION_ERROR;
  }
  H[0][0] = m_problem->evalDerivativeByArgs(x, derDegrees);

  dataMultiIndex[0] = 0;
  dataMultiIndex[1] = 2;
  derDegrees->setData(2, dataMultiIndex);

  H[1][1] = m_problem->evalDerivativeByArgs(x, derDegrees);

  dataMultiIndex[0] = 1;
  dataMultiIndex[1] = 1;
  derDegrees->setData(2, dataMultiIndex);

  H[0][1] = H[1][0] = m_problem->evalDerivativeByArgs(x, derDegrees);
  delete derDegrees;
  return RC::SUCCESS;
}

RC Solver::solveSystemHesssian(double *const *H, const double *b, const double *x)
{
  //Coming soon...
  return RC::SUCCESS;
}


RC Solver::NewtonMethod(const IVector *x_0, const IVector *params, double tolerance)
{
  IVector* x = x_0->clone();
  IVector* grad = x_0->clone();
  double** H = static_cast<double**>(malloc(sizeof(double*) * 2));
  if (!checkPointer(getLogger(), (void*) x) || !checkPointer(getLogger(), (void*) grad) || !checkPointer(getLogger(), (void*) H)) {
    delete x;
    delete grad;
    delete H;
    return RC::ALLOCATION_ERROR;
  }
  H[0] = static_cast<double*>(malloc(sizeof(double) * 2));
  H[1] = static_cast<double*>(malloc(sizeof(double) * 2));

  if (!checkPointer(getLogger(), (void*) H[0]) || !checkPointer(getLogger(), (void*) H[1])) {
    delete x;
    delete grad;
    delete H[0];
    delete H[1];
    delete H;
    return RC::ALLOCATION_ERROR;
  }

  unsigned k = 0;
  RC rc;
  do {
    rc = m_problem->evalGradientByArgs(x, grad);
    if (rc != RC::SUCCESS) {
      break;
    }
    rc = calcHessian(x, H);


  } while (x->norm(IVector::NORM::SECOND) > tolerance || k < m_maxStepsNum);

  //Coming soon
  return RC::SUCCESS;
}


RC Solver::getSolution(IVector *&solution) const
{
  if (m_solution == nullptr) {
    solution = nullptr;
    return RC::SUCCESS;
  }
  solution = m_solution->clone();
  return solution == nullptr ? RC::ALLOCATION_ERROR : RC::SUCCESS;
}

bool Solver::isValidArgsDomain(const ICompact *const &args) const
{
  if (!checkPointer(getLogger(), (void*)args)) {
    return false;
  }
  if (m_problem == nullptr) {
    LogError(getLogger(), RC::NO_PROBLEM_SET);
    return false;
  }
  IVector* left = nullptr;
  RC rc = args->getLeftBoundary(left);
  if (rc != RC::SUCCESS) {
    LogError(getLogger(), rc);
    return false;
  }

  IVector* right = nullptr;
  rc = args->getRightBoundary(right);
  if (rc != RC::SUCCESS) {
    LogError(getLogger(), rc);
    delete left;
    return false;
  }

  bool answer = m_problem->isValidArgs(left) && m_problem->isValidArgs(right);
  delete left;
  delete right;
  return answer;
}

bool Solver::isValidParamsDomain(const ICompact *const &params) const
{
  if (!checkPointer(getLogger(), (void*)params)) {
    return false;
  }

  if (m_problem == nullptr) {
    LogError(getLogger(), RC::NO_PROBLEM_SET);
    return false;
  }

  IVector* left = nullptr;
  RC rc = params->getLeftBoundary(left);
  if (rc != RC::SUCCESS) {
    LogError(getLogger(), rc);
    return false;
  }

  IVector* right = nullptr;
  rc = params->getRightBoundary(right);
  if (rc != RC::SUCCESS) {
    LogError(getLogger(), rc);
    delete left;
    return false;
  }

  bool answer = m_problem->isValidParams(left) && m_problem->isValidParams(right);
  delete left;
  delete right;
  return answer;
}

RC Solver::setArgsDomain(const ICompact *const &args)
{
  if (!checkPointer(getLogger(), (void*)args)) {
    return RC::NULLPTR_ERROR;
  }
  if (m_problem == nullptr) {
    LogError(getLogger(), RC::NO_PROBLEM_SET);
    return RC::NO_PROBLEM_SET;
  }
  if (!isValidArgsDomain(args)) {
    LogError(getLogger(), RC::INVALID_ARGUMENT);
    return RC::INVALID_ARGUMENT;
  }

  ICompact* tmpArgs = args->clone();

  if(tmpArgs == nullptr) {
    LogError(getLogger(), RC::ALLOCATION_ERROR);
    return RC::ALLOCATION_ERROR;
  }
  delete m_args;
  m_args = tmpArgs;
  return RC::SUCCESS;
}

RC Solver::setParamsDomain(const ICompact *const &params)
{
  if (!checkPointer(getLogger(), (void*)params)) {
    return RC::NULLPTR_ERROR;
  }
  if (m_problem == nullptr) {
    LogError(getLogger(), RC::NO_PROBLEM_SET);
    return RC::NO_PROBLEM_SET;
  }
  if (!isValidParamsDomain(params)) {
    LogError(getLogger(), RC::INVALID_ARGUMENT);
    return RC::INVALID_ARGUMENT;
  }

  ICompact* tmpParams = params->clone();

  if(tmpParams == nullptr) {
    LogError(getLogger(), RC::ALLOCATION_ERROR);
    return RC::ALLOCATION_ERROR;
  }
  delete m_params;
  m_params = tmpParams;
  return RC::SUCCESS;
}

RC Solver::setProblem(const IDiffProblem *const &pProblem)
{
  if (!checkPointer(getLogger(), (void*)pProblem)) {
    return RC::NULLPTR_ERROR;
  }
  IDiffProblem* tmpProblem = pProblem->clone();
  if(tmpProblem == nullptr) {
    LogError(getLogger(), RC::ALLOCATION_ERROR);
    return RC::ALLOCATION_ERROR;
  }
  delete m_problem;
  m_problem = tmpProblem;
  return RC::SUCCESS;
}

RC Solver::solveByArgs(const IVector *const &initArg, IVector *const *const &solverParams)
{
  if (!checkPointer(getLogger(), (void*)initArg)
  || !checkPointer(getLogger(), (void*)solverParams)
  || !checkPointer(getLogger(), (void*)solverParams[0])
  || !checkPointer(getLogger(), (void*)solverParams[1])) {
    return RC::NULLPTR_ERROR;
  }
  if (m_problem == nullptr) {
    LogError(getLogger(), RC::NO_PROBLEM_SET);
    return RC::NO_PROBLEM_SET;
  }
  if (m_args == nullptr) {
    LogError(getLogger(), RC::NO_ARGS_SET);
    return RC::NO_ARGS_SET;
  }

  if (!m_args->isInside(initArg) || m_problem->isValidParams(solverParams[0])|| solverParams[1]->getDim() != m_methodParamsNum)
  {
    LogError(getLogger(), RC::INVALID_ARGUMENT);
    return RC::INVALID_ARGUMENT;
  }
  const double* paramsGradMethod = solverParams[1]->getData();
  double eps = paramsGradMethod[0];
  return NewtonMethod(initArg, solverParams[0], eps);

}




