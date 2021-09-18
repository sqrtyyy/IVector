//
// Created by aleksei on 10.09.2021.
//

#include <ISolver.h>

#include "Solver.h"

#include "../Helpers/Helpers.h"

const unsigned Solver::m_methodParamsNum = 1;
const unsigned Solver::m_maxStepsNum = 100;
ILogger* Solver::m_pLogger = nullptr;

Solver::Solver()
: m_problem(nullptr)
, m_args(nullptr)
, m_params(nullptr)
, m_solution(nullptr)
{}
RC Solver::calcHessian(IVector const* const x, double * const* const H) const
{
  auto * dataMultiIndex = static_cast<size_t *>(calloc(x->getDim(), sizeof(size_t)));
  if (dataMultiIndex == nullptr) {
    return RC::ALLOCATION_ERROR;
  }
  IMultiIndex* derDegrees = IMultiIndex::createMultiIndex(x->getDim(), dataMultiIndex);
  if (derDegrees == nullptr) {
    free(dataMultiIndex);
    return RC::ALLOCATION_ERROR;
  }
  for(size_t i = 0; i < x->getDim(); ++i) {
    for (size_t j = 0; j < x->getDim(); ++j) {
      dataMultiIndex[i]++;
      dataMultiIndex[j]++;
      derDegrees->setData(x->getDim(), dataMultiIndex);
      H[i][j] = m_problem->evalDerivativeByArgs(x, derDegrees);
      dataMultiIndex[i] = 0;
      dataMultiIndex[j] = 0;
    }
  }
  free(dataMultiIndex);
  delete derDegrees;
  return RC::SUCCESS;
}

void Solver::deleteHessian(double **& H, size_t dim){
  for (size_t i = 0; i < dim; ++i) {
    free(H[i]);
  }
  free(H);
  H = nullptr;
}

RC Solver::solveSystemHessianGauss(double *const *H, const double *b_, double *x, size_t dim)
{
  auto * b = static_cast<double*>(malloc(dim *sizeof(double*)));
  if (b == nullptr) {
    return RC::ALLOCATION_ERROR;
  }

  double tolerance = 0.0001;
  for (size_t k = 0; k < dim; ++k) {
    double max = std::abs(H[k][k]);
    size_t index = k;
    for (size_t i = k + 1; i < dim; i++) {
      if (std::abs(H[i][k]) > max) {
        max = std::abs(H[i][k]);
        index = i;
      }
    }
    for (int j = 0; j < dim; j++) {
      std::swap(H[k][j], H[index][j]);
    }
    std::swap(b[k], b[index]);

    for (size_t i = k; i < dim; i++)
    {
      double tmp = H[i][k];
      if (std::abs(tmp) < tolerance) continue;
      for (int j = 0; j < dim; j++)
        H[i][j] = H[i][j] / tmp;
      b[i] = b[i] / tmp;
      if (i == k)  continue;
      for (int j = 0; j < dim; j++)
        H[i][j] = H[i][j] - H[k][j];
      b[i] = b[i] - b[k];
    }
  }
  for (size_t k = dim - 1; k >= 0; k--)
  {
    x[k] = b[k];
    for (int i = 0; i < k; i++)
      b[i] = b[i] - H[i][k] * x[k];
  }
  delete b;
  return RC::SUCCESS;
}

RC Solver::NewtonMethod(const IVector *x_0, const IVector *params, double tolerance, bool isArgs)
{
  IVector* x = x_0->clone();
  IVector* grad = x_0->clone();
  auto* buffer_x = static_cast<double*>(malloc(x_0->getDim() *sizeof(double*)));
  auto** H = static_cast<double**>(calloc(x_0->getDim(), sizeof(double*)));

  if (x == nullptr || grad == nullptr || buffer_x == nullptr || H == nullptr) {
    LogError(getLogger(), RC::ALLOCATION_ERROR);
    delete buffer_x;
    delete x;
    delete grad;
    delete H;
    return RC::ALLOCATION_ERROR;
  }
  for (size_t i = 0; i < x->getDim(); ++i) {
    H[i] = static_cast<double*>(malloc(x->getDim() *sizeof(double*)));
    if (H[i] == nullptr) {
      LogError(getLogger(), RC::ALLOCATION_ERROR);
      deleteHessian(H, x->getDim());
      delete buffer_x;
      delete x;
      delete grad;
      return RC::ALLOCATION_ERROR;
    }
  }
  unsigned k = 0;
  RC rc;
  do {
    rc = isArgs ? m_problem->evalGradientByArgs(x, grad) : m_problem->evalGradientByParams(x, grad);
    if (rc != RC::SUCCESS) {
      break;
    }
    grad->applyFunction([](double x) {return -x;});

    rc = calcHessian(x, H);
    if(rc != RC::SUCCESS) break;

    rc = solveSystemHessianGauss(H, grad->getData(), buffer_x, x->getDim());
    if(rc != RC::SUCCESS) break;
    size_t i = 0;
    rc = x->applyFunction([&](double x){return x + buffer_x[i++] * tolerance / 2;});
  } while (grad->norm(IVector::NORM::SECOND) > tolerance && k < m_maxStepsNum && rc == RC::SUCCESS);

  if (rc == RC::SUCCESS) {
    if (grad->norm(IVector::NORM::CHEBYSHEV) > tolerance) {
      LogError(getLogger(), RC::VECTOR_NOT_FOUND);
      rc = RC::VECTOR_NOT_FOUND;
    }
    if (m_solution != nullptr) {
      delete m_solution;
      m_solution = nullptr;
    }
    m_solution = x->clone();
    if (m_solution == nullptr) {
      LogError(getLogger(), RC::ALLOCATION_ERROR);
      rc = RC::ALLOCATION_ERROR;
    }
  }

  deleteHessian(H, x->getDim());
  delete buffer_x;
  delete x;
  delete grad;

  return rc;
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

ISolver *Solver::clone() const
{
  auto* newSolver = new(std::nothrow) Solver();
  if (newSolver == nullptr) {
    LogError(getLogger(), RC::ALLOCATION_ERROR);
    return nullptr;
  }
  if (m_args) {
    if (newSolver->setArgsDomain(m_args) != RC::SUCCESS) {
      delete newSolver;
      return nullptr;
    }
  }
  if (m_params) {
    if (newSolver->setParamsDomain(m_params) != RC::SUCCESS) {
      delete newSolver;
      return nullptr;
    }
  }
  if (m_problem) {
    if (newSolver->setProblem(m_problem) != RC::SUCCESS) {
      delete newSolver;
      return nullptr;
    }
  }
  if (m_solution) {
    newSolver->m_solution = m_solution->clone();
    if (newSolver->m_solution == nullptr) {
      delete newSolver;
      LogError(getLogger(), RC::ALLOCATION_ERROR);
      return nullptr;
    }
  }
  return newSolver;
}

Solver::~Solver()
{
  delete m_problem;
  delete m_solution;
  delete m_args;
  delete m_params;
}

RC Solver::solveByParams(const IVector *const &initParam, IVector *const *const &solverParams)
{
  if (!checkPointer(getLogger(), (void*)initParam)
      || !checkPointer(getLogger(), (void*)solverParams)
      || !checkPointer(getLogger(), (void*)solverParams[0])
      || !checkPointer(getLogger(), (void*)solverParams[1])) {
    return RC::NULLPTR_ERROR;
  }
  if (m_problem == nullptr) {
    LogError(getLogger(), RC::NO_PROBLEM_SET);
    return RC::NO_PROBLEM_SET;
  }
  if (m_params == nullptr) {
    LogError(getLogger(), RC::NO_ARGS_SET);
    return RC::NO_ARGS_SET;
  }

  if (!m_params->isInside(initParam) || m_problem->isValidArgs(solverParams[0])|| solverParams[1]->getDim() != m_methodParamsNum)
  {
    LogError(getLogger(), RC::INVALID_ARGUMENT);
    return RC::INVALID_ARGUMENT;
  }
  const double* paramsGradMethod = solverParams[1]->getData();
  double eps = paramsGradMethod[0];
  return NewtonMethod(initParam, solverParams[0], eps, false);
}

///ISolver
ISolver* ISolver::createSolver()
{
  return new(std::nothrow) Solver();
}

RC ISolver::setLogger(ILogger *const pLogger)
{
  Solver::m_pLogger = pLogger;
  return RC::SUCCESS;
}

ILogger *ISolver::getLogger()
{
  return Solver::m_pLogger;
}




