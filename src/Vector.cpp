//
// Created by aleksei on 3/20/21.
//

#include <cstring>
#include <cstdlib>
#include <cmath>
#include "Vector.h"

#define LogError(Logger, Code)  if((Logger) != nullptr) (Logger)->severe((Code), __FILE__, __func__, __LINE__)

ILogger* Vector::m_pLogger = nullptr;

Vector* Vector::createVector(size_t dim, const double * const & ptr_data) {
  if(dim == 0) {
    return nullptr;
  }
  if (!ptr_data){
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return nullptr;
  }
  for (int i = 0; i < dim; ++i) {
    if (std::isnan(ptr_data[i])){
      LogError(m_pLogger, RC::NOT_NUMBER);
      return nullptr;
    } else if (std::isinf(ptr_data[i])) {
      LogError(m_pLogger, RC::INFINITY_OVERFLOW);
      return nullptr;
    }
  }
  size_t size = sizeof(Vector) + dim * sizeof(double);
  char* mem = static_cast<char*>(malloc(size));
  if(mem == nullptr) return nullptr;
  auto* vec = new(mem) Vector(dim);
  auto data = (double *)(mem + sizeof(Vector));
  for (int i = 0; i < vec->m_dim; ++i) {
    data[i] = ptr_data[i];
  }
  return vec;
}

Vector::Vector(size_t dim) {
  m_dim = dim;
}

RC Vector::applyFunction(const std::function<double(double)> &fun) {
  double* data = getRawData();
  auto buffer = (double*) std::malloc(m_dim * sizeof(double ));
  if(buffer != nullptr){
    std::memcpy(buffer, data, m_dim * sizeof(double));
  } else {
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return RC::ALLOCATION_ERROR;
  }
  int i;
  for (i = 0; i < m_dim; ++i) {
    buffer[i] = fun(data[i]);
    if(std::isnan(buffer[i])) break;
  }
  if (i == m_dim){
    memcpy(data, buffer, sizeof(double) * m_dim );
  }
  free(buffer);
  return RC::SUCCESS;
}

IVector *Vector::clone() const {
  return createVector(m_dim, getRawData());
}

RC Vector::dec(const IVector *const &op) {
  if (!op){
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  double* data = getRawData();
  if(op->getDim() != m_dim) {
    LogError(m_pLogger, RC::INVALID_ARGUMENT);
    return RC::INVALID_ARGUMENT;
  }
  for (int i = 0; i < m_dim; ++i) {
    data[i] -= op->getData()[i];
  }
  return RC::SUCCESS;
}

RC Vector::foreach(const std::function<void(double)> &fun) const {
  double* data = getRawData();
  for (int i = 0; i < m_dim; ++i) {
    fun(data[i]);
  }
  return RC::SUCCESS;
}

RC Vector::getCord(size_t index, double &val) const {
  if(index >= m_dim) {
    LogError(m_pLogger, RC::INDEX_OUT_OF_BOUND);
    return  RC::INDEX_OUT_OF_BOUND;
  }
  val = getRawData()[index];
  return RC::SUCCESS;
}

const double *Vector::getData() const {
  return getRawData();
}

size_t Vector::getDim() const {
  return m_dim;
}

RC Vector::inc(const IVector *const &op) {
  if (!op){
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  double* data = getRawData();
  if(op->getDim() != m_dim) {
    LogError(m_pLogger, RC::INVALID_ARGUMENT);
    return RC::INVALID_ARGUMENT;
  }
  for (int i = 0; i < m_dim; ++i) {
    data[i] += op->getData()[i];
  }
  return RC::SUCCESS;
}

double Vector::norm(IVector::NORM n) const {
  auto data = getRawData();
  double norm = 0;
  switch (n) {
    case NORM::CHEBYSHEV:
      norm = std::abs(data[0]);
      for (int i = 1; i < m_dim; ++i) {
        if(norm < std::abs(data[i])){
          norm = std::abs(data[i]);
        }
      }
      break;
    case NORM::FIRST:
      for (int i = 0; i < m_dim; ++i) {
        norm += fabs(data[i]);
      }
      break;
    case NORM::SECOND:
      for (int i = 0; i < m_dim; ++i) {
        norm += fabs(data[i]) * fabs(data[i]);
      }
      norm = std::sqrt(norm);
      break;
    case NORM::AMOUNT:
      norm = NAN;
      LogError(m_pLogger, RC::INVALID_ARGUMENT);
      break;
  }
  return norm;
}

RC Vector::scale(double multiplier) {
  return applyFunction([multiplier](double val){ return val * multiplier;});
}

RC Vector::setCord(size_t index, double val) {
  if(index >= m_dim) {
    LogError(m_pLogger, RC::INDEX_OUT_OF_BOUND);
    return RC::INDEX_OUT_OF_BOUND;
  }
  if(std::isnan(val)) {
    LogError(m_pLogger, RC::INVALID_ARGUMENT);
    return RC::INVALID_ARGUMENT;
  }
  getRawData()[index] = val;
  return RC::SUCCESS;
}

size_t Vector::sizeAllocated() const {
  return m_dim * sizeof(double);
}

RC Vector::copyInstance(IVector *const dest, const IVector *const &src) {
  if (!dest || !src) return RC::NULLPTR_ERROR;
  if (dest->getDim() != src->getDim()) return RC::MISMATCHING_DIMENSIONS;
  if (src == dest) return RC::SUCCESS;
  if (std::abs(src - dest) * sizeof(void*) < src->sizeAllocated() ||
  std::abs(src - dest) * sizeof(void*) < dest->sizeAllocated()) {
    LogError(m_pLogger, RC::MEMORY_INTERSECTION);
    return RC::MEMORY_INTERSECTION;
  }
  for (int i = 0; i < dest->getDim(); ++i) {
    dest->setCord(i, src->getData()[i]);
  }
  return RC::SUCCESS;
}

RC Vector::moveInstance(IVector *const dest, IVector *&src) {
  RC error = copyInstance(dest, src);
  if(error == RC::SUCCESS){
    delete src;
    src = nullptr;
  } else {
    LogError(m_pLogger, error);
  }
  return error;
}

double *Vector::getRawData() const {
  return (double *) ((char*)this + sizeof(Vector));
}

RC Vector::setLogger(ILogger *const logger) {
  if(logger == nullptr) return RC::NULLPTR_ERROR;
  m_pLogger = logger;
  return RC::SUCCESS;
}

IVector *Vector::add(const IVector *const &op1, const IVector *const &op2) {
  if (!op1 || !op2){
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return nullptr;
  }
  if(op1->getDim() != op2->getDim()){
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return nullptr;
  }
  auto buf = (double*) malloc(sizeof(double) * op1->getDim());
  Vector* vector = nullptr;
  if(buf != nullptr) {
    auto data1 = op1->getData();
    auto data2 = op2->getData();
    for (int i = 0; i < op1->getDim(); ++i) {
      buf[i] = data1[i] + data2[i];
    }
    vector = createVector(op1->getDim(), buf);
    free(buf);
    if(vector == nullptr){
      LogError(m_pLogger, RC::ALLOCATION_ERROR);
    }
  } else {
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
  }
  return vector;
}
IVector *Vector::sub(const IVector *const &op1, const IVector *const &op2) {
  if (!op1 || !op2){
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return nullptr;
  }
  if(op1->getDim() != op2->getDim()){
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return nullptr;
  }
  Vector* vector = nullptr;
  auto buf = (double*) calloc(op1->getDim(), sizeof(double));
  if(buf != nullptr) {
    for (int i = 0; i < op1->getDim(); ++i) {
      buf[i] = op1->getData()[i] - op2->getData()[i];
    }
    vector = createVector(op1->getDim(), buf);
    free(buf);
    if(vector == nullptr ){
      LogError(m_pLogger, RC::ALLOCATION_ERROR);
    }
  } else {
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
  }
  return vector;
}

double Vector::dot(const IVector *const &op1, const IVector *const &op2) {
  if (!op1 || !op2){
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return NAN;
  }
  if(op1->getDim() != op2->getDim()){
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return NAN;
  }
  double result = 0;
  auto data1 = op1->getData();
  auto data2 = op2->getData();
  for (int i = 0; i < op1->getDim(); ++i) {
    result += data1[i] * data2[i];
  }
  if (m_pLogger != nullptr) m_pLogger->info(RC::SUCCESS);
  return result;
}

bool Vector::equals(const IVector *const &op1, const IVector *const &op2, IVector::NORM n, double tol) {
  if (!op1 || !op2){
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return false;
  }
  if ((n == NORM::AMOUNT || tol < 0)) {
    LogError(m_pLogger, RC::INVALID_ARGUMENT);
    return false;
  }
  auto subVector = Vector::sub(op1, op2);
  bool  answer = false;
  if(subVector != nullptr){
    double norm_sub = subVector->norm(n);
    delete subVector;
    answer = norm_sub <= tol;
  } else {
    LogError(m_pLogger, RC::INVALID_ARGUMENT);
  }
  return  answer;
}

RC Vector::setData(size_t dim, const double *const &ptr_data) {
  if(!ptr_data){
    LogError(m_pLogger, RC::NULLPTR_ERROR);
  }
  if(dim != m_dim){
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
  }
  memcpy((char*)getRawData(), (char*)ptr_data, dim * sizeof(double));
  return RC::SUCCESS;
}

Vector::~Vector() = default;

IVector::~IVector() = default;

IVector* IVector::createVector(size_t dim, double const* const& ptr_data){
  return Vector::createVector(dim, ptr_data);
}

RC IVector::copyInstance(IVector* const dest, IVector const* const& src){
  return Vector::copyInstance(dest, src);
}

RC IVector::moveInstance(IVector* const dest, IVector*& src){
  return Vector::moveInstance(dest, src);
}

RC IVector::setLogger(ILogger* const logger){
  return Vector::setLogger(logger);
}

IVector* IVector::add(IVector const* const& op1, IVector const* const& op2){
  return Vector::add(op1, op2);
}

IVector* IVector::sub(IVector const* const& op1, IVector const* const& op2){
  return Vector::sub(op1, op2);
}

double IVector::dot(IVector const* const& op1, IVector const* const& op2){
  return Vector::dot(op1, op2);
}

bool IVector::equals(IVector const* const& op1, IVector const* const& op2, NORM n, double tol){
  return Vector::equals(op1, op2, n, tol);
}
