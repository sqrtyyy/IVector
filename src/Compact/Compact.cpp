//
// Created by aleksei on 22.08.2021.
//

#include <cstring>
#include <utility>
#include <ICompact.h>

#include "Compact.h"
#include "../Helpers/Helpers.h"

ILogger* Compact::m_pLogger = nullptr;
ILogger* Compact::Iterator::m_pIteratorLogger = nullptr;

Compact::Compact(unsigned dim) : m_dim(dim){}

bool Compact::isValidMultiIdx(const IMultiIndex *index) const
{
  if (index->getDim() != getDim()) return false;
  for (unsigned i = 0; i < getDim(); ++i) {
    size_t val1;
    if(index->getAxisIndex(i, val1) != RC::SUCCESS) return false;
    if (getGridRawData()[i] < val1) return false;
  }
  return true;
}

double *Compact::getLeftData() const
{
  return reinterpret_cast<double*>((char*)(this) + sizeof(Compact));
}

double *Compact::getRightData() const
{
  return getLeftData() + getDim();
}

size_t *Compact::getGridRawData() const
{
  return reinterpret_cast<size_t*>(getRightData() + getDim());
}


size_t Compact::getDim() const
{
  return m_dim;
}

IMultiIndex *Compact::getGrid() const
{
  return IMultiIndex::createMultiIndex(m_dim, getGridRawData());
}

const size_t *Compact::getGridData() const
{
  return getGridRawData();
}


RC Compact::getLeftBoundary(IVector *&vec) const
{
  vec = IVector::createVector(m_dim, getLeftData());
  return vec != nullptr ? RC::SUCCESS : RC::ALLOCATION_ERROR;
}

RC Compact::getRightBoundary(IVector *&vec) const
{
  vec = IVector::createVector(m_dim, getRightData());
  return vec != nullptr ? RC::SUCCESS : RC::ALLOCATION_ERROR;
}

bool Compact::isInside(const IVector *const &vec) const
{
  if (!checkPointer(m_pLogger, (void *) vec)) return false;
  if (vec->getDim() != getDim()) {
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return false;
  }
  for (unsigned i = 0; i < getDim(); ++i) {
    if (getLeftData()[i] > vec->getData()[i] || vec->getData()[i] > getRightData()[i] ) return false;
  }
  return true;
}

RC Compact::getVectorCoords(const IMultiIndex *index, IVector *const &val) const
{
  if (!checkPointer(m_pLogger, (void *) val) || !checkPointer(m_pLogger, (void *) index)) return RC::NULLPTR_ERROR;
  if (val->getDim() != getDim() || index->getDim() != getDim()) {
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return RC::MISMATCHING_DIMENSIONS;
  }
  if (!isValidMultiIdx(index)) {
    LogError(m_pLogger, RC::INDEX_OUT_OF_BOUND);
    return RC::INDEX_OUT_OF_BOUND;
  }
  auto* leftData = getLeftData();
  auto* rightData = getRightData();
  auto* sizes = getGridRawData();
  auto* idxs = index->getData();

  for (unsigned i = 0; i < getDim(); ++i) {
    val->setCord(i, leftData[i] + (rightData[i] - leftData[i]) * (double)idxs[i] / (double)sizes[i]);
  }
  return RC::SUCCESS;
}

RC Compact::getVectorCopy(const IMultiIndex *index, IVector *&val) const
{
  if (!isValidMultiIdx(index)) {
    LogError(m_pLogger, RC::INDEX_OUT_OF_BOUND);
    return RC::INDEX_OUT_OF_BOUND;
  }
  val = IVector::createVector(getDim(), getLeftData());
  if (val == nullptr) {
    return RC::ALLOCATION_ERROR;
  }
  RC res = getVectorCoords(index, val);
  if (res != RC::SUCCESS) {
    delete val;
    val = nullptr;
  }
  return res;
}

ICompact *Compact::clone() const
{
  return ICompact::createCompact(IVector::createVector(m_dim, getLeftData()),
                                 IVector::createVector(m_dim, getRightData()),
                                 IMultiIndex::createMultiIndex(m_dim, getGridRawData()));
}

ICompact * Compact::createIntersection(const ICompact *op1, const ICompact *op2, const IMultiIndex *const grid, double tol)
{
  if (!checkPointer(m_pLogger, (void *) op1)
  || !checkPointer(m_pLogger, (void *) op2)
  || !checkPointer(m_pLogger, (void *) grid)){
    return nullptr;
  }
  if (op1->getDim() != op2->getDim()) {
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return nullptr;
  }
  IVector* leftOp1Vec = nullptr;
  IVector* rightOp1Vec = nullptr;
  IVector* leftOp2Vec = nullptr;
  IVector* rightOp2Vec = nullptr;

  if ( op1->getLeftBoundary(leftOp1Vec) != RC::SUCCESS ||
       op1->getRightBoundary(rightOp1Vec) != RC::SUCCESS ||
       op1->getLeftBoundary(leftOp2Vec) != RC::SUCCESS ||
       op1->getRightBoundary(rightOp2Vec) != RC::SUCCESS) {
    delete leftOp1Vec;
    delete rightOp1Vec;
    delete leftOp2Vec;
    delete rightOp2Vec;
    return nullptr;
  }

  IVector* intersecLeft = nullptr;
  IVector* intersecRight = nullptr;
  IMultiIndex* intersecGrid = nullptr;
  if ((intersecLeft = leftOp1Vec->clone()) == nullptr ||
      (intersecRight = leftOp1Vec->clone()) == nullptr ||
      (intersecGrid = grid->clone()) == nullptr) {
    delete leftOp1Vec;
    delete rightOp1Vec;
    delete leftOp2Vec;
    delete rightOp2Vec;
    return nullptr;
  }
  auto* leftOp1Data = leftOp1Vec->getData();
  auto* rightOp1Data = rightOp1Vec->getData();
  auto* leftOp2Data = leftOp2Vec->getData();
  auto* rightOp2Data = rightOp2Vec->getData();
  bool isIntersec = true;
  for (unsigned i = 0; i < op1->getDim() && isIntersec; ++i) {
    isIntersec = false;
    if (leftOp1Data[i] >= leftOp2Data[i] &&  leftOp1Data[i] <= rightOp2Data[i]) {
      intersecLeft->setCord(i, leftOp1Data[i]);
      intersecRight->setCord(i, std::min(rightOp1Data[i],rightOp2Data[i]));
      isIntersec = true;
    }
    if (leftOp2Data[i] >= leftOp1Data[i] &&  leftOp2Data[i] <= rightOp1Data[i]) {
      intersecLeft->setCord(i, leftOp2Data[i]);
      intersecRight->setCord(i, std::min(rightOp1Data[i],rightOp2Data[i]));
      isIntersec = true;
    }
    if (intersecRight->getData()[i] - intersecLeft->getData()[i] < tol) {
      intersecRight->setCord(i, intersecLeft->getData()[i]);
      intersecGrid->setAxisIndex(i, 0);
    }
  }
  delete leftOp1Vec;
  delete rightOp1Vec;
  delete leftOp2Vec;
  delete rightOp2Vec;

  ICompact* intersection = nullptr;

  if (isIntersec) {
    intersection = ICompact::createCompact(intersecLeft, intersecRight, intersecGrid);
  }
  delete intersecLeft;
  delete intersecRight;
  delete intersecGrid;

  return intersection;
}

ICompact *Compact::createCompactSpan(const ICompact *op1, const ICompact *op2, const IMultiIndex *grid)
{
  if (!checkPointer(m_pLogger, (void *) op1)
  || !checkPointer(m_pLogger, (void *) op2)
  || !checkPointer(m_pLogger, (void *) grid)){
    return nullptr;
  }
  if (op1->getDim() != op2->getDim()) {
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return nullptr;
  }
  IVector* leftOp1Vec = nullptr;
  IVector* rightOp1Vec = nullptr;
  IVector* leftOp2Vec = nullptr;
  IVector* rightOp2Vec = nullptr;

  if ( op1->getLeftBoundary(leftOp1Vec) != RC::SUCCESS ||
  op1->getRightBoundary(rightOp1Vec) != RC::SUCCESS ||
  op1->getLeftBoundary(leftOp2Vec) != RC::SUCCESS ||
  op1->getRightBoundary(rightOp2Vec) != RC::SUCCESS) {
    delete leftOp1Vec;
    delete rightOp1Vec;
    delete leftOp2Vec;
    delete rightOp2Vec;
    return nullptr;
  }

  IVector* spanLeft = nullptr;
  IVector* spanRight = nullptr;
  if ((spanLeft = leftOp1Vec->clone()) == nullptr ||
      (spanRight = leftOp1Vec->clone()) == nullptr) {
    delete leftOp1Vec;
    delete rightOp1Vec;
    delete leftOp2Vec;
    delete rightOp2Vec;
    return nullptr;
  }

  for (unsigned i = 0; i < op1->getDim(); ++i) {
    spanLeft->setCord(i, std::min(leftOp1Vec->getData()[i], leftOp2Vec->getData()[i]));
    spanRight->setCord(i, std::min(rightOp1Vec->getData()[i], rightOp2Vec->getData()[i]));
  }

  ICompact* span = ICompact::createCompact(spanLeft, spanRight, grid);

  delete spanLeft;
  delete spanRight;

  delete leftOp1Vec;
  delete rightOp1Vec;
  delete leftOp2Vec;
  delete rightOp2Vec;

  return span;
}

ICompact *Compact::createCompact(const IVector *vec1, const IVector *vec2, const IMultiIndex *nodeQuantities)
{
  if (!checkPointer(m_pLogger, (void *) vec1)
  || !checkPointer(m_pLogger, (void *) vec2)
  || !checkPointer(m_pLogger, (void *) nodeQuantities)){
    return nullptr;
  }
  if (vec1->getDim() != vec2->getDim() || vec1->getDim() != nodeQuantities->getDim()) {
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return nullptr;
  }
  unsigned  dim = vec1->getDim();
  size_t size = sizeof(Compact) + 2 * dim * sizeof(double) + dim * sizeof(size_t);
  char* mem = static_cast<char*>(malloc(size));
  if(mem == nullptr) {
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return nullptr;
  }
  auto* compact = new(mem) Compact(dim);
  for (unsigned i = 0; i < dim; ++i) {
    compact->getLeftData()[i] = std::min(vec1->getData()[i], vec2->getData()[i]);
    compact->getRightData()[i] = std::max(vec1->getData()[i], vec2->getData()[i]);
  }
  memcpy(compact->getGridRawData(), nodeQuantities->getData(), dim * sizeof(size_t));
  return compact;
}

ICompact::IIterator *Compact::getBegin(const IMultiIndex *const &bypassOrder) const
{
  return nullptr;
}

ICompact::IIterator *Compact::getEnd(const IMultiIndex *const &bypassOrder) const
{
  return nullptr;
}

ICompact::IIterator *Compact::getIterator(const IMultiIndex *const &index, const IMultiIndex *const &bypassOrder) const
{
  return nullptr;
}











//ICompact

ICompact *ICompact::createCompact(const IVector *vec1, const IVector *vec2, const IMultiIndex *nodeQuantities)
{
  return Compact::createCompact(vec1, vec2, nodeQuantities);
}

ICompact *ICompact::createIntersection(const ICompact *op1, const ICompact *op2, const IMultiIndex *const grid, double tol)
{
  return Compact::createIntersection(op1, op2, grid, tol);
}

ICompact *ICompact::createCompactSpan(const ICompact *op1, const ICompact *op2, const IMultiIndex *const grid)
{
  return Compact::createCompactSpan(op1, op2, grid);
}

RC ICompact::setLogger(ILogger *const logger)
{
  Compact::m_pLogger = logger;
  return RC::SUCCESS;
}
ILogger* ICompact::getLogger()
{
  return Compact::m_pLogger;
}
Compact::~Compact() = default;

ICompact::~ICompact() = default;


//CompactControlBlock



CompactControlBlock::CompactControlBlock(Compact *compact) : m_pCompact(compact)
{}

RC CompactControlBlock::get(const IMultiIndex *const &currentIndex, IVector *const &val) const
{
  if (m_pCompact == nullptr) {
    return RC::SOURCE_COMPACT_DESTROYED;
  }
  return m_pCompact->getVectorCoords(currentIndex, val);
}

RC CompactControlBlock::get(IMultiIndex *const &currentIndex, const IMultiIndex *const &bypassOrder) const
{
  if (!checkPointer(nullptr, (void *) currentIndex, false) ||
      !checkPointer(nullptr, (void *) bypassOrder, false)){
    return RC::NULLPTR_ERROR;
  }
  if (m_pCompact == nullptr) {
    return RC::SOURCE_COMPACT_DESTROYED;
  }
  if (m_pCompact->getDim() != currentIndex->getDim() || m_pCompact->getDim() != bypassOrder->getDim()) {
    return RC::MISMATCHING_DIMENSIONS;
  }
  bool  isSet = false;
  for (unsigned i = 0; i < bypassOrder->getDim() && !isSet; ++i) {
    unsigned idx = bypassOrder->getData()[i];
    if(idx > m_pCompact->getDim()) {
      return RC::INDEX_OUT_OF_BOUND;
    }
    isSet = currentIndex->setAxisIndex(idx, currentIndex->getData()[idx] + 1) == RC::SUCCESS;
  }
  return isSet ? RC::SUCCESS : RC::INDEX_OUT_OF_BOUND;
}

RC CompactControlBlock::getCopy(const IMultiIndex *const &currentIndex, IVector *&val) const
{
  if (m_pCompact == nullptr) {
    return RC::SOURCE_COMPACT_DESTROYED;
  }
  return m_pCompact->getVectorCopy(currentIndex, val);
}

bool CompactControlBlock::hasNext(const IMultiIndex *const &currentIndex, const IMultiIndex *const &bypassOrder) const
{
  if (!checkPointer(nullptr, (void *) currentIndex, false) ||
  !checkPointer(nullptr, (void *) bypassOrder, false)){
    return false;
  }
  if (m_pCompact == nullptr) {
    return false;
  }
  if (m_pCompact->getDim() != currentIndex->getDim() || m_pCompact->getDim() != bypassOrder->getDim()) {
    return false;
  }
  bool  hasNext = false;
  for (unsigned i = 0; i < bypassOrder->getDim() && !hasNext; ++i) {
    unsigned idx = bypassOrder->getData()[i];
    if(idx > m_pCompact->getDim()) {
      return false;
    }
    hasNext = currentIndex->getData()[idx] + 1 >= m_pCompact->getGridData()[idx];
  }
  return hasNext;
}

ICompactControlBlock::~ICompactControlBlock() = default;


//Iterator
Compact::Iterator::Iterator(std::shared_ptr<CompactControlBlock> controlBlock, IMultiIndex *idx, IMultiIndex *bypassOrder)
: m_controlBlock(std::move(controlBlock))
, m_curIdx(idx)
, m_bypassOrder(bypassOrder)
{}
ICompact::IIterator *Compact::Iterator::getNext()
{
  IMultiIndex* newIdx = nullptr;
  IMultiIndex* newByPass = nullptr;
  if((newIdx = m_curIdx->clone()) == nullptr ||
     (newByPass = m_bypassOrder->clone()) == nullptr) {
    delete newIdx;
    delete newByPass;
    return nullptr;
  }
  RC rc = m_controlBlock->get(newIdx, newByPass);
  if (rc != RC::SUCCESS) {
    LogError(m_pIteratorLogger, rc);
    delete newIdx;
    delete newByPass;
    return nullptr;
  }
  return new Iterator(m_controlBlock, newIdx, newByPass);
}

ICompact::IIterator *Compact::Iterator::clone() const
{
  IMultiIndex* newIdx = nullptr;
  IMultiIndex* newByPass = nullptr;
  if((newIdx = m_curIdx->clone()) == nullptr ||
  (newByPass = m_bypassOrder->clone()) == nullptr) {
    delete newIdx;
    delete newByPass;
    return nullptr;
  }
  return new Iterator(m_controlBlock, newIdx, newByPass);
}

RC Compact::Iterator::getVectorCoords(IVector *const &val) const
{
  auto rc = m_controlBlock->get(m_curIdx, val);
  if (rc != RC::SUCCESS) {
    LogError(m_pIteratorLogger, rc);
  }
  return rc;
}

RC Compact::Iterator::getVectorCopy(IVector *&val) const
{
  auto rc = m_controlBlock->getCopy(m_curIdx, val);
  if (rc != RC::SUCCESS) {
    LogError(m_pIteratorLogger, rc);
  }
  return rc;
}

bool Compact::Iterator::isValid() const
{
  return m_controlBlock->hasNext(m_curIdx, m_bypassOrder);
}

RC Compact::Iterator::next()
{
  RC rc = m_controlBlock->get(m_curIdx, m_bypassOrder);
  if (rc != RC::SUCCESS) {
    LogError(m_pIteratorLogger, rc);
  }
  return rc;
}

Compact::Iterator::~Iterator()
{
  delete m_curIdx;
  delete m_bypassOrder;
}

ICompact::IIterator::~IIterator() = default;

RC ICompact::IIterator::setLogger(ILogger *const pLogger)
{
  Compact::Iterator::m_pIteratorLogger = pLogger;
  return RC::SUCCESS;
}

ILogger *ICompact::IIterator::getLogger()
{
  return Compact::Iterator::m_pIteratorLogger;
}



