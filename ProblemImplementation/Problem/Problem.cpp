//
// Created by aleksei on 10.09.2021.
//

#include <cmath>
#include "Problem.h"
#include "../Helpers/Helpers.h"

ILogger*     Problem::m_pLogger = nullptr;
const size_t Problem::m_argsNum = 2;
const size_t Problem::m_paramsNum = 4;

Problem::Problem()
: m_args(nullptr)
, m_argsCompact(nullptr)
, m_params(nullptr)
, m_paramsCompact(nullptr)
{}

double Problem::evaluate(const double *params, const double *args)
{
  const double x = args[0];
  const double y = args[1];

  return  sin(params[0] * x + params[1] * y) + params[2] * x * x  - params[3] * y * y; // sin(Ax + By) + Cx^2 - Dy^2
}

double Problem::evaluateDerivative(const size_t * degrees, const double *params, const double *args)
{
  const size_t dx = degrees[0];
  const size_t dy = degrees[1];
  const size_t sum = dx + dy;

  const double x = args[0];
  const double y = args[1];

  const double arg = params[0] * x + params[1] * y;
  const double sinDer = sum % 4 == 0 ? sin(arg) : sum % 3 == 0 ? -cos(arg) : sum % 2 == 0 ? -sin(arg) : cos(arg);
  const double sinMult = (dx == 0 ? 1 : (double)dx * params[0]) * (dy == 0 ? 1 : (double)dy * params[1]);
  const double newC = params[1] * (dx > 2 ? 0 : 2);
  const double newD = params[2] * (dy > 2 ? 0 : 2);

  return sinMult * sinDer + newC * (dx == 0 ? x * x : dx == 1 ? x : dx == 2 ? 1 : 0) - newD * (dy == 0 ? y * y : dy == 1 ? y : dy == 2 ? 1 : 0);
}

void Problem::evalGradient(IVector *const &val, const double *params, const double *args)
{
  const double x = args[0];
  const double y = args[1];

  val->setCord(0,  cos(params[0] * x + params[1] * y) + 2 * params[2] * x);
  val->setCord(1, cos(params[0] * x + params[1] * y) - 2 * params[3] * x);
}

RC Problem::setParams(const IVector *const &params)
{
  if (!checkPointer(getLogger(), (void *) params)) {
    return RC::NULLPTR_ERROR;
  }
  if (params->getDim() != m_paramsCompact->getDim()) {
    LogError(getLogger(), RC::MISMATCHING_DIMENSIONS);
    return RC::MISMATCHING_DIMENSIONS;
  }
  if(m_params == nullptr) {
    m_params = params->clone();
    if (m_params == nullptr) {
      LogError(getLogger(), RC::ALLOCATION_ERROR);
      return RC::ALLOCATION_ERROR;
    }
  }
  return IVector::copyInstance(m_params, params);
}

RC Problem::setArgs(const IVector *const &args)
{
  if (!checkPointer(getLogger(), (void *) args)) {
    return RC::NULLPTR_ERROR;
  }
  if (args->getDim() != m_argsCompact->getDim()) {
    LogError(getLogger(), RC::MISMATCHING_DIMENSIONS);
    return RC::MISMATCHING_DIMENSIONS;
  }
  if(m_args == nullptr) {
    m_args = args->clone();
    if (m_args == nullptr) {
      LogError(getLogger(), RC::ALLOCATION_ERROR);
      return RC::ALLOCATION_ERROR;
    }
  }
  return IVector::copyInstance(m_args, args);
}

bool Problem::isValidParams(const IVector *const &params) const
{
  return m_paramsCompact->isInside(params);
}

bool Problem::isValidArgs(const IVector *const &args) const
{
  return m_argsCompact->isInside(args);
}

double Problem::evalByParams(const IVector *const &params) const
{
  if(!checkPointer(getLogger(), (void *) params)) {
    return NAN;
  }
  if (params->getDim() != m_paramsCompact->getDim()) {
    LogError(getLogger(), RC::MISMATCHING_DIMENSIONS);
    return NAN;
  }
  if (isValidParams(params)) {
    LogError(getLogger(), RC::INVALID_ARGUMENT);
    return NAN;
  }
  if (m_args == nullptr) {
    LogError(getLogger(), RC::NO_ARGS_SET);
    return NAN;
  }
  return evaluate(params->getData(), m_args->getData());
}

double Problem::evalByArgs(const IVector *const &args) const
{
  if (!checkPointer(getLogger(), (void*)args)) {
    LogError(getLogger(), RC::NULLPTR_ERROR);
    return NAN;
  }
  if (args->getDim() != m_argsCompact->getDim()) {
    LogError(getLogger(), RC::MISMATCHING_DIMENSIONS);
    return NAN;
  }
  if (isValidArgs(args)) {
    LogError(getLogger(), RC::INVALID_ARGUMENT);
    return NAN;
  }
  if (m_params == nullptr) {
    LogError(getLogger(), RC::NO_PARAMS_SET);
    return NAN;
  }
  return evaluate(m_params->getData(), args->getData());
}

IDiffProblem *Problem::clone() const
{
  IDiffProblem* newProblem = IDiffProblem::createDiffProblem();
  if (newProblem == nullptr) {
    LogError(getLogger(), RC::ALLOCATION_ERROR);
    return nullptr;
  }
  RC rc = newProblem->setArgs(m_args);
  if (rc != RC::SUCCESS && rc != RC::NULLPTR_ERROR) {
    delete newProblem;
    return nullptr;
  }

  rc = newProblem->setParams(m_params);
  if (rc != RC::SUCCESS && rc != RC::NULLPTR_ERROR) {
    delete newProblem;
    return nullptr;
  }
  return newProblem;
}

Problem::~Problem()
{
  delete m_params;
  delete m_args;
  delete m_argsCompact;
  delete m_paramsCompact;
}

double Problem::evalDerivativeByArgs(const IVector *const &args, const IMultiIndex *const &index) const
{
  if(!checkPointer(getLogger(), (void *) args) || !checkPointer(getLogger(), (void*) index)) {
    return NAN;
  }
  if (index->getDim() != m_argsCompact->getDim() || args->getDim() != m_argsCompact->getDim()) {
    LogError(getLogger(), RC::MISMATCHING_DIMENSIONS);
    return NAN;
  }
  if (!isValidArgs(args)) {
    LogError(getLogger(), RC::INVALID_ARGUMENT);
    return NAN;
  }
  if (m_params == nullptr) {
    LogError(getLogger(), RC::NO_PARAMS_SET);
    return NAN;
  }
  return evaluateDerivative(index->getData(), m_params->getData(), args->getData());
}

double Problem::evalDerivativeByParams(const IVector *const &params, const IMultiIndex *const &index) const
{
  if(!checkPointer(getLogger(), (void *) params) || !checkPointer(getLogger(), (void*) index)) {
    return NAN;
  }
  if (index->getDim() != m_paramsCompact->getDim() || params->getDim() != m_paramsCompact->getDim()) {
    LogError(getLogger(), RC::MISMATCHING_DIMENSIONS);
    return NAN;
  }
  if (!isValidParams(params)) {
    LogError(getLogger(), RC::INVALID_ARGUMENT);
    return NAN;
  }
  if (m_args == nullptr) {
    LogError(getLogger(), RC::NO_ARGS_SET);
    return NAN;
  }
  return evaluateDerivative(index->getData(), params->getData(), m_args->getData());
}

RC Problem::evalGradientByArgs(const IVector *const &args, IVector *const &val) const
{
  if(!checkPointer(getLogger(), (void *) args) || !checkPointer(getLogger(), (void*) val)) {
    return RC::NULLPTR_ERROR;
  }
  if (val->getDim() != m_argsCompact->getDim() || args->getDim() != m_argsCompact->getDim()) {
    LogError(getLogger(), RC::MISMATCHING_DIMENSIONS);
    return RC::MISMATCHING_DIMENSIONS;
  }
  if (!isValidArgs(args)) {
    LogError(getLogger(), RC::INVALID_ARGUMENT);
    return RC::INVALID_ARGUMENT;
  }
  if (m_params == nullptr) {
    LogError(getLogger(), RC::NO_PARAMS_SET);
    return RC::NO_PARAMS_SET;
  }
  evalGradient(val, m_params->getData(), args->getData());
  return RC::SUCCESS;
}

RC Problem::evalGradientByParams(const IVector *const &params, IVector *const &val) const
{
  if(!checkPointer(getLogger(), (void *) params) || !checkPointer(getLogger(), (void*) val)) {
    return RC::NULLPTR_ERROR;
  }
  if (val->getDim() != m_argsCompact->getDim() || params->getDim() != m_paramsCompact->getDim()) {
    LogError(getLogger(), RC::MISMATCHING_DIMENSIONS);
    return RC::MISMATCHING_DIMENSIONS;
  }
  if (!isValidParams(params)) {
    LogError(getLogger(), RC::INVALID_ARGUMENT);
    return RC::INVALID_ARGUMENT;
  }
  if (m_args == nullptr) {
    LogError(getLogger(), RC::NO_ARGS_SET);
    return RC::NO_ARGS_SET;
  }
  evalGradient(val, params->getData(), m_args->getData());
  return RC::SUCCESS;
}

RC Problem::setParamsDomain(const ICompact *const &params)
{
  if (!checkPointer(IDiffProblem::getLogger(), (void *) params)) {
    return RC::NULLPTR_ERROR;
  }

  ICompact *copy = params->clone();
  if(copy == nullptr) {
    LogError(IDiffProblem::getLogger(), RC::ALLOCATION_ERROR);
    return RC::ALLOCATION_ERROR;
  }
  delete m_paramsCompact;
  m_paramsCompact = copy;
  if (m_params != nullptr && !m_paramsCompact->isInside(m_params)) {
    delete m_params;
    m_params = nullptr;
  }
  return RC::SUCCESS;
}

RC Problem::setArgsDomain(const ICompact *const &args, ILogger *logger)
{
  if (!checkPointer(IDiffProblem::getLogger(), (void *) args)) {
    return RC::NULLPTR_ERROR;
  }

  ICompact *copy = args->clone();
  if(copy == nullptr) {
    LogError(IDiffProblem::getLogger(), RC::ALLOCATION_ERROR);
    return RC::ALLOCATION_ERROR;
  }
  delete m_argsCompact;
  m_argsCompact = copy;
  if (m_args != nullptr && !m_argsCompact->isInside(m_args)) {
    delete m_args;
    m_args = nullptr;
  }
  return RC::SUCCESS;
}

IDiffProblem *IDiffProblem::createDiffProblem()
{
  return new(std::nothrow) Problem();
}

IProblem* IProblem::createProblem()
{
  return IDiffProblem::createDiffProblem();
}

RC IProblem::setLogger(ILogger *const logger)
{
  Problem::m_pLogger = logger;
  return RC::SUCCESS;
}

ILogger* IProblem::getLogger()
{
  return Problem::m_pLogger;
}

IProblem::~IProblem() = default;

RC IDiffProblem::setLogger(ILogger *const logger)
{
  Problem::m_pLogger = logger;
  return RC::SUCCESS;
}

ILogger* IDiffProblem::getLogger()
{
  return Problem::m_pLogger;
}

IDiffProblem::~IDiffProblem() = default;
