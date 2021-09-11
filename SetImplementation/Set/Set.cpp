//
// Created by aleksei on 4/3/21.
//

#include <cstdlib>
#include <cstring>
#include "Set.h"

#define LogError(Logger, Code)  if((Logger) != nullptr) (Logger)->severe((Code), __FILE__, __func__, __LINE__)

ISet::~ISet() = default;
ILogger* Set::m_pLogger = nullptr;

RC Set::reallocate() {
  size_t new_capacity = m_capacity == 0 ? 1 : m_capacity * 2;
  auto* tmp_data = static_cast<double *>(malloc(new_capacity * sizeof(double) * m_dim));
  auto* tmp_codes = static_cast<size_t *>(malloc(new_capacity * sizeof(size_t)));
  if (tmp_data == nullptr || tmp_codes == nullptr){
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return RC::ALLOCATION_ERROR;
  }

  memcpy(tmp_data, m_pData, m_size * sizeof(double) * m_dim);
  free(m_pData);
  memcpy(tmp_codes, m_codes, m_size * sizeof(size_t));
  free(m_codes);
  m_pData = tmp_data;
  m_codes = tmp_codes;
  m_capacity = new_capacity;
  return RC::SUCCESS;
}

Set::Set() : m_dim(0), m_size(0), m_pData(nullptr), m_capacity(0), m_codes(nullptr),
             m_curCode(0), m_pControlBlock(new(std::nothrow) SetControlBlock(this)){};

RC Set::insert(IVector const * const& val, IVector::NORM n, double tol) {
  if (val == nullptr) {
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  if(m_size == 0) {
    m_dim = val->getDim();
  }
  if(m_capacity < m_size + 1) {
    RC result = reallocate();
    if(result != RC::SUCCESS) return result;
  }
  RC result = findFirst(val, n, tol);
  if (result == RC::VECTOR_NOT_FOUND){
    memcpy((char*)m_pData + m_size * m_dim * sizeof(double), (char*)val->getData(), m_dim  * sizeof(double));
    m_curCode++;
    m_codes[m_size] = m_curCode;
    m_size++;
  }
  return result == RC::VECTOR_NOT_FOUND ? RC::SUCCESS : result == RC::SUCCESS ? RC::VECTOR_ALREADY_EXIST : result;
}

RC Set::getIdx(const IVector *const &pat, IVector::NORM n, double tol, size_t& idx) const {
  if(pat == nullptr) {
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  IVector* val = IVector::createVector(pat->getDim(), pat->getData());
  if(val == nullptr) {
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return RC::ALLOCATION_ERROR;
  }
  for (idx = 0; idx < m_size; ++idx) {
    val->setData(m_dim, m_pData + idx * m_dim);
    val->dec(pat);
    if(val->norm(n) < tol){
      break;
    }
  }
  delete val;
  return idx < m_size ? RC::SUCCESS : RC::VECTOR_NOT_FOUND;
}

RC Set::remove(size_t index) {
  if (index >= m_size) {
    LogError(m_pLogger, RC::INDEX_OUT_OF_BOUND);
    return RC::INDEX_OUT_OF_BOUND;
  }
  memmove((char*)m_pData + m_dim * sizeof(double) * index,
          (char*)m_pData + m_dim * sizeof(double) * (index + 1),
          m_dim * sizeof(double) * (m_size - index - 1));
  memmove((char*)m_codes + sizeof(size_t) * index,
          (char*)m_codes + sizeof(size_t) * (index + 1),
          sizeof(size_t) * (m_size - index - 1));
  m_size--;
  if(m_size == 0) {
    m_curCode = 0;
  }
  return RC::SUCCESS;
}

RC Set::remove(IVector const * const& pat, IVector::NORM n, double tol) {
  if (pat == nullptr) {
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  if (m_size == 0) {
    if (m_pLogger) m_pLogger->info(RC::VECTOR_NOT_FOUND);
    return RC::VECTOR_NOT_FOUND;
  }
  size_t idx = 0;
  while (getIdx(pat, n, tol, idx) != RC::VECTOR_NOT_FOUND ){
    remove(idx);
  }
  return RC::SUCCESS;

}

size_t Set::getSize() const {
  return m_size;
}

size_t Set::getDim() const {
  return m_dim;
}
RC Set::getCopy(size_t index, IVector *&val) const {
  if(index >= m_size) {
    LogError(m_pLogger, RC::INDEX_OUT_OF_BOUND);
    return RC::INDEX_OUT_OF_BOUND;
  }
  val = IVector::createVector(m_dim, m_pData + index * m_dim);
  if (val == nullptr) {
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return RC::ALLOCATION_ERROR;
  }
  return RC::SUCCESS;
}

RC Set::getCoords(size_t index, IVector *const &val) const {
  if (val == nullptr) {
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  if (index >= m_size) {
    LogError(m_pLogger, RC::INDEX_OUT_OF_BOUND);
    return RC::INDEX_OUT_OF_BOUND;
  }
  if (val->getDim() != m_dim) {
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return RC::MISMATCHING_DIMENSIONS;
  }
  return val->setData(m_dim, m_pData + index * m_dim);
}

RC Set::findFirst(const IVector *const &pat, IVector::NORM n, double tol) const {
  if(pat == nullptr) {
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  IVector* val = IVector::createVector(pat->getDim(), pat->getData());
  RC result = findFirstAndCopyCoords(pat, n, tol, val);
  delete val;
  return result;
}

RC Set::findFirstAndCopyCoords(const IVector *const &pat, IVector::NORM n, double tol, IVector *const &val) const {
  if(pat == nullptr || val == nullptr) {
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  if (m_size == 0) {
    if (m_pLogger) m_pLogger->info(RC::VECTOR_NOT_FOUND);
    return RC::VECTOR_NOT_FOUND;
  }
  if(pat->getDim() != m_dim || val->getDim() != m_dim) {
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return RC::MISMATCHING_DIMENSIONS;
  }
  bool isFound = false;
  for (size_t i = 0; i < m_size; ++i) {
    val->setData(m_dim, m_pData + i * m_dim);
    val->dec(pat);
    if(val->norm(n) < tol){
      val->inc(pat);
      isFound = true;
      break;
    }
  }
  if(!isFound) {
    if (m_pLogger) m_pLogger->info(RC::VECTOR_NOT_FOUND);
    return RC::VECTOR_NOT_FOUND;
  }
  return RC::SUCCESS;
}

RC Set::findFirstAndCopy(IVector const * const& pat, IVector::NORM n, double tol, IVector *& val) const {
  if(pat == nullptr) {
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  if(m_size == 0) {
    if (m_pLogger) m_pLogger->info(RC::VECTOR_NOT_FOUND);
    return RC::VECTOR_NOT_FOUND;
  }
  val = IVector::createVector(m_dim, m_pData);
  RC result = findFirstAndCopyCoords(pat, n, tol, val);
  if (result != RC::SUCCESS) {
    delete val;
    val = nullptr;
    return result;
  }
  return RC::SUCCESS;
}

ISet* Set::clone() const {
  Set* newSet = new(std::nothrow) Set();
  if(newSet == nullptr){
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return nullptr;
  }
  newSet->m_dim = m_dim;
  newSet->m_size = m_size;
  newSet->m_capacity = m_capacity;
  newSet->m_pData = static_cast<double *>(malloc(m_capacity * m_dim * sizeof(double)));
  if(newSet->m_pData == nullptr) {
    delete newSet;
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return nullptr;
  }
  memcpy(static_cast<void*>(newSet->m_pData), static_cast<void*>(m_pData), m_size * m_dim * sizeof(double));

  newSet->m_codes = static_cast<size_t *>(malloc(m_capacity * sizeof(size_t)));
  if(newSet->m_codes == nullptr) {
    delete newSet;
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return nullptr;
  }
  memcpy(static_cast<void*>(newSet->m_codes), static_cast<void*>(m_codes), m_size * sizeof(size_t));

  return newSet;
}

RC ISet::setLogger(ILogger* const logger) {
    Set::m_pLogger = logger;
    return RC::SUCCESS;
}

RC Set::beginVec(IVector *vector, size_t &code) const {
  if (m_size == 0) return RC::SUCCESS;
  if (vector == nullptr) {
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  vector->setData(m_dim, m_pData);
  code = m_codes[0];
  return RC::SUCCESS;
}

RC Set::endVec(IVector *vector, size_t &code) const {
  if (m_size == 0) return RC::SUCCESS;
  if (vector == nullptr) {
    LogError(m_pLogger, RC::NULLPTR_ERROR);
    return RC::NULLPTR_ERROR;
  }
  vector->setData(m_dim, m_pData + m_dim * (m_size - 1));
  code = m_codes[m_size - 1];
  return RC::SUCCESS;
}

RC Set::nextVec(IVector *vector, size_t &code, size_t indexInc) const {
  if(code == -1){
    LogError(m_pLogger, RC::SET_INDEX_OVERFLOW);
    return RC::SET_INDEX_OVERFLOW;
  }
  size_t idx = 0;
  while (idx < m_size && code < m_codes[idx]){
    idx++;
  }
  if(idx + indexInc - 1 > m_size) {
    code = -1;
    LogError(m_pLogger, RC::SET_INDEX_OVERFLOW);
    return RC::SET_INDEX_OVERFLOW;
  }
  code = m_codes[idx + indexInc - 1];
  vector->setData(m_dim, m_pData + m_dim * (idx + indexInc - 1));
  return RC::SUCCESS;
}

RC Set::prevVec(IVector *vector, size_t &code, size_t indexDec) const {
  if(code == -1) {
    LogError(m_pLogger, RC::SET_INDEX_OVERFLOW);
    return RC::SET_INDEX_OVERFLOW;
  }
  size_t idx = 0;
  while (idx < m_size && code > m_codes[idx]){
    idx++;
  }
  if(idx - indexDec + 1 > m_size) {
    code = -1;
    LogError(m_pLogger, RC::SET_INDEX_OVERFLOW);
    return RC::SET_INDEX_OVERFLOW;
  }
  code = m_codes[idx - indexDec + 1];
  vector->setData(m_dim, m_pData + m_dim * (idx - indexDec + 1));
  return RC::SUCCESS;
}

ISet::IIterator *Set::getBegin() const {
  if(m_size == 0) return nullptr;
  IVector* vec = IVector::createVector(m_dim, m_pData);
  if(vec == nullptr) {
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return nullptr;
  }
  auto* iterator = new(std::nothrow) Iterator(vec,m_pControlBlock, m_codes[0]);
  if(iterator == nullptr){
    if(m_pLogger) m_pLogger->warning(RC::ALLOCATION_ERROR);
    delete vec;
    return nullptr;
  }
  return iterator;
}

ISet::IIterator *Set::getEnd() const {
  if(m_size == 0) return nullptr;
  IVector* vec = IVector::createVector(m_dim, m_pData + m_dim * (m_size - 1));
  if(vec == nullptr) {
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return nullptr;
  }
  auto* iterator = new(std::nothrow) Iterator(vec,m_pControlBlock, m_codes[m_size - 1]);
  if(iterator == nullptr){
    if(m_pLogger) m_pLogger->warning(RC::ALLOCATION_ERROR);
    delete vec;
    return nullptr;
  }
  return iterator;
}

ISet *ISet::makeIntersection(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) {
  if (op1 == nullptr || op2 == nullptr) {
    LogError(Set::m_pLogger, RC::NULLPTR_ERROR);
    return nullptr;
  }
  if (op1->getDim() != op2->getDim()) {
    LogError(Set::m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return nullptr;
  }
  size_t size1 = op1->getSize();
  size_t size2 = op2->getSize();
  if (size1 == 0)
    return op2->clone();
  if (op2->getSize() == 0)
    return op1->clone();
  IVector* vec1;
  if(op1->getCopy(0, vec1) != RC::SUCCESS){
    return nullptr;
  }
  IVector* vec2;
  if(op2->getCopy(0, vec2) != RC::SUCCESS){
    return nullptr;
  }
  ISet* intersection = ISet::createSet();
  for (size_t i = 0; i < size1; ++i) {
    op1->getCoords(i, vec1);
    for (size_t j = 0; j < size2; ++j) {
      op2->getCoords(j, vec2);
      vec2->dec(vec1);
      if (vec2->norm(n) < tol) {
        RC rc;
        if ((rc = intersection->insert(vec1, n, tol)) != RC::SUCCESS && rc != RC::VECTOR_ALREADY_EXIST) {
          delete vec1;
          delete vec2;
          delete intersection;
          return nullptr;
        }
      }
    }
  }
  delete vec1;
  delete vec2;
  return intersection;
}

ISet* ISet::makeUnion(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {
  if (op1 == nullptr || op2 == nullptr) {
    LogError(Set::m_pLogger, RC::NULLPTR_ERROR);
    return nullptr;
  }
  if (op1->getDim() != op2->getDim()) {
    LogError(Set::m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return nullptr;
  }
  size_t size1 = op1->getSize();
  size_t size2 = op2->getSize();
  if (size1 == 0)
    return op2->clone();
  if (op2->getSize() == 0)
    return op1->clone();
  IVector* vec1;
  if(op1->getCopy(0, vec1) != RC::SUCCESS){
    return nullptr;
  }
  IVector* vec2;
  if(op2->getCopy(0, vec2) != RC::SUCCESS){
    return nullptr;
  }
  ISet* _union_ = ISet::createSet();
  for (size_t i = 0; i < size1; ++i) {
    RC rc;
    op1->getCoords(i, vec1);
    if ((rc = _union_->insert(vec1, n, tol)) != RC::SUCCESS && rc != RC::VECTOR_ALREADY_EXIST ) {
      delete vec1;
      delete vec2;
      delete _union_;
      return nullptr;
    }
    for (size_t j = 0; j < size2; ++j) {
      op2->getCoords(j, vec2);
      if ((rc = _union_->insert(vec2, n, tol)) != RC::SUCCESS && rc != RC::VECTOR_ALREADY_EXIST) {
        delete vec1;
        delete vec2;
        delete _union_;
        return nullptr;
      }
    }
  }
  delete vec1;
  delete vec2;
  return _union_;
}

ISet* ISet::sub(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {
  if (op1 == nullptr || op2 == nullptr) {
    LogError(Set::m_pLogger, RC::NULLPTR_ERROR);
    return nullptr;
  }
  if (op1->getDim() != op2->getDim()) {
    LogError(Set::m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return nullptr;
  }
  ISet* sub = op1->clone();
  if(sub == nullptr) {
    LogError(Set::m_pLogger, RC::ALLOCATION_ERROR);
    return nullptr;
  }
  size_t size = op2->getSize();
  IVector* vec = nullptr;
  RC res;
  if((res = op2->getCopy(0, vec)) != RC::SUCCESS){
    LogError(Set::m_pLogger, res);
    return nullptr;
  }
  for (size_t i = 0; i < size; ++i) {
    op2->getCoords(i, vec);
    sub->remove(vec, n, tol);
  }
  delete vec;
  return sub;
}

ISet* ISet::symSub(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {
  return sub(ISet::makeUnion(op1, op2, n, tol), ISet::makeIntersection(op1, op2, n, tol), n, tol);
}

bool ISet::equals(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {
  if (op1 == nullptr || op2 == nullptr) {
    LogError(Set::m_pLogger, RC::NULLPTR_ERROR);
    return false;
  }
  if (op1->getDim() != op2->getDim()) {
    LogError(Set::m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return false;
  }
  size_t size1 = op1->getSize();
  size_t size2 = op2->getSize();
  if (size1 == 0)
    return op2->clone();
  if (op2->getSize() == 0)
    return op1->clone();
  IVector* vec1;
  RC res;
  if((res = op1->getCopy(0, vec1)) != RC::SUCCESS){
    LogError(Set::m_pLogger, res);
    return false;
  }
  IVector* vec2;
  if((res = op2->getCopy(0, vec2)) != RC::SUCCESS){
    LogError(Set::m_pLogger, res);
    return false;
  }
  for (size_t i = 0; i < size1; i++) {
    bool isFound = false;
    op1->getCoords(i, vec1);
    for (size_t j = 0; j < size2; j++) {
      op2->getCoords(j, vec2);
      vec2->dec(vec1);
      if (vec2->norm(n) <= tol) {
        isFound = true;
        break;
      }
    }
    if (!isFound) {
      delete vec1;
      delete vec2;
      return false;
    }
  }

  for (size_t i = 0; i < size2; i++) {
    bool isFound = false;
    op2->getCoords(i, vec1);
    for (size_t j = 0; j < size1; j++) {
      op1->getCoords(j, vec2);
      vec2->dec(vec1);
      if (vec2->norm(n) <= tol) {
        isFound = true;
        break;
      }
    }
    if (!isFound) {
      delete vec1;
      delete vec2;
      return false;
    }
  }
  delete vec1;
  delete vec2;
  return true;
}

bool ISet::subSet(ISet const * const& op1, ISet const * const& op2, IVector::NORM n, double tol) {
  return equals(ISet::makeUnion(op1, op2, n, tol), op1, n, tol);
}

ISet *ISet::createSet() {
  return new Set();
}

Set::~Set() {
  m_pControlBlock->deleteSet();
  free(m_pData);
  free(m_codes);
}


ILogger* Set::Iterator::m_pIteratorLogger = nullptr;

Set::Iterator::Iterator(IVector *vec,
                        std::shared_ptr<ISetControlBlock> p_ControlBlock,
                        size_t code) : m_pVector(vec), m_code(code),m_pIteratorControlBlock(std::move(p_ControlBlock)){}

ISet::IIterator * Set::Iterator::clone() const{
    IVector* copyVector = m_pVector->clone();
    if(copyVector == nullptr && m_pIteratorLogger) {
        m_pIteratorLogger->warning(RC::ALLOCATION_ERROR);
        return nullptr;
    }
    auto* newIterator = new(std::nothrow) Set::Iterator(copyVector, m_pIteratorControlBlock, m_code);
    if (newIterator == nullptr) {
        if(m_pIteratorLogger) m_pIteratorLogger->warning(RC::ALLOCATION_ERROR);
        delete copyVector;
    }
    return newIterator;
}

ISet::IIterator *Set::Iterator::getNext(size_t indexInc) const {
    IIterator* iterator = clone();
    if(iterator != nullptr) {
        iterator->next(indexInc);
    }
    return iterator;
}
ISet::IIterator * Set::Iterator::getPrevious(size_t indexInc) const {
    IIterator* iterator = clone();
    if(iterator != nullptr) {
        iterator->previous(indexInc);
    }
    return iterator;
}

RC Set::Iterator::next(size_t indexInc) {
    return m_pIteratorControlBlock->getNext(m_pVector, m_code, indexInc);
}

RC Set::Iterator::previous(size_t indexInc) {
    return m_pIteratorControlBlock->getPrevious(m_pVector, m_code,  indexInc);
}

RC Set::Iterator::getVectorCoords(IVector *const &val) const {
    if(val == nullptr){
        if(m_pIteratorLogger) m_pIteratorLogger->warning(RC::NULLPTR_ERROR);
        return RC::NULLPTR_ERROR;
    }
    if (val->getDim() != m_pVector->getDim()) {
        if(m_pIteratorLogger) m_pIteratorLogger->warning(RC::NULLPTR_ERROR);
        return RC::MISMATCHING_DIMENSIONS;
    }
    return val->setData(m_pVector->getDim(), m_pVector->getData());
}

RC Set::Iterator::getVectorCopy(IVector *&val) const {
    val = m_pVector->clone();
    if (val == nullptr) {
        if(m_pIteratorLogger) {
            m_pIteratorLogger->warning(RC::ALLOCATION_ERROR);
        }
        return RC::ALLOCATION_ERROR;
    }
    return RC::SUCCESS;
}

bool Set::Iterator::isValid() const {
    return m_code == -1;
}

RC Set::Iterator::makeBegin() {
    return m_pIteratorControlBlock->getBegin(m_pVector, m_code);
}

RC Set::Iterator::makeEnd() {
    return m_pIteratorControlBlock->getEnd(m_pVector, m_code);
}

RC Set::Iterator::setLogger(ILogger *const pLogger) {
    Set::Iterator::m_pIteratorLogger = pLogger;
    return RC::SUCCESS;
}

ISet::IIterator::~IIterator() = default;

Set::Iterator::~Iterator() {
    delete m_pVector;
}

RC ISet::IIterator::setLogger(ILogger *const pLogger) {
    Set::Iterator::m_pIteratorLogger = pLogger;
    return RC::SUCCESS;
}
ISet::IIterator *Set::getIterator(size_t index) const {
    if (index >= m_size) {
        if(m_pLogger) m_pLogger->warning(RC::INDEX_OUT_OF_BOUND);
        return nullptr;
    }
    IVector* vec = IVector::createVector(m_dim, m_pData + m_dim * index);
    IIterator* iterator = new(std::nothrow) Set::Iterator(vec,m_pControlBlock, m_codes[index]);
    if (iterator == nullptr) {
        if (m_pLogger) m_pLogger->warning(RC::ALLOCATION_ERROR);
        delete vec;
        return nullptr;
    }
    return iterator;
}

SetControlBlock::SetControlBlock(const Set * pSet) : m_pSet(pSet){}


RC SetControlBlock::getBegin(IVector *const &vec, size_t &index) const {
    if(m_pSet == nullptr){
        return RC::SOURCE_SET_DESTROYED;
    }
    return m_pSet->beginVec(vec, index);
}

RC SetControlBlock::getEnd(IVector *const &vec, size_t &index) const {
    if(m_pSet == nullptr){
        return RC::SOURCE_SET_DESTROYED;
    }
    return m_pSet->endVec(vec, index);
}

RC SetControlBlock::getNext(IVector *const &vec, size_t &index, size_t indexInc) const {
    if(m_pSet == nullptr){
        return RC::SOURCE_SET_DESTROYED;
    }
    return m_pSet->nextVec(vec, index, indexInc);
}

RC SetControlBlock::getPrevious(IVector *const &vec, size_t &index, size_t indexInc) const {
    if(m_pSet == nullptr){
        return RC::SOURCE_SET_DESTROYED;
    }
    return m_pSet->prevVec(vec, index, indexInc);
}


void SetControlBlock::deleteSet() {
    m_pSet = nullptr;
}

ISetControlBlock::~ISetControlBlock() = default;


