//
// Created by aleksei on 22.08.2021.
//

#include <cstring>
#include <new>
#include "MultiIndex.h"
#include "../Helpers/Helpers.h"

ILogger* MultiIndex::m_pLogger = nullptr;

//_____________________________________
//MultiIndex
MultiIndex::MultiIndex(unsigned int dim)
: m_dim(dim)
{}

size_t *MultiIndex::getIndices() const
{
  return (size_t*)((char*)this + sizeof(MultiIndex));
}

IMultiIndex *MultiIndex::createMultiIndex(size_t dim, const size_t *indices)
{

  if (!checkPointer(m_pLogger, (void *) indices, true)) return nullptr;

  if (dim == 0) {
    LogError(m_pLogger, RC::INVALID_ARGUMENT);
    return nullptr;
  }
  char* memory = static_cast<char *>(malloc(sizeof(MultiIndex) + sizeof(size_t) * dim));
  if (memory == nullptr) {
    LogError(m_pLogger, RC::ALLOCATION_ERROR);
    return nullptr;
  }
  auto * multiIndex = new(memory) MultiIndex(dim);
  memcpy(multiIndex->getIndices(), indices, dim);
  return multiIndex;
}

bool MultiIndex::isValidIndex(size_t idx, bool isLog) const
{
  if (idx > m_dim) {
    if (isLog) LogError(m_pLogger, RC::INDEX_OUT_OF_BOUND);
    return false;
  }
  return true;
}

//_____________________________________
//MultiIndex : IMultiIndex

size_t MultiIndex::getDim() const
{
  return m_dim;
}

const size_t *MultiIndex::getData() const
{
  return getIndices();
}

IMultiIndex *MultiIndex::clone() const
{
  return nullptr;
}

RC MultiIndex::getAxisIndex(size_t axisIndex, size_t &val) const
{
  if(!isValidIndex(axisIndex, true)) return RC::INDEX_OUT_OF_BOUND;
  val = getIndices()[axisIndex];
  return RC::SUCCESS;
}


RC MultiIndex::incAxisIndex(size_t axisIndex, ssize_t val)
{
  if(!isValidIndex(axisIndex, true)) return RC::INDEX_OUT_OF_BOUND;

  getIndices()[axisIndex] += val;
  return RC::SUCCESS;
}

RC MultiIndex::setData(size_t dim, const size_t *const &ptr_data)
{
  if (!checkPointer(m_pLogger, (void *) ptr_data, true)) return RC::NULLPTR_ERROR;
  if (m_dim != dim) {
    LogError(m_pLogger, RC::MISMATCHING_DIMENSIONS);
    return RC::MISMATCHING_DIMENSIONS;
  }

  memcpy((char*) getIndices(), (char*) ptr_data, dim * sizeof(size_t));
  return RC::SUCCESS;
}

RC MultiIndex::setAxisIndex(size_t axisIndex, size_t val)
{
  if(!isValidIndex(axisIndex, true)) return RC::INDEX_OUT_OF_BOUND;
  getIndices()[axisIndex] = val;
  return RC::SUCCESS;
}

RC MultiIndex::setLogger(ILogger *const pLogger)
{
  if(checkPointer(pLogger, m_pLogger, true)){
    m_pLogger = pLogger;
    return RC::SUCCESS;
  }
  return RC::NULLPTR_ERROR;
}

ILogger *MultiIndex::getLogger()
{
  return m_pLogger;
}

MultiIndex::~MultiIndex() = default;

//_____________________________________
//IMultiIndex
IMultiIndex * IMultiIndex::createMultiIndex(size_t dim, const size_t* indices){
  return MultiIndex::createMultiIndex(dim, indices);
}
RC IMultiIndex::setLogger(ILogger * const pLogger){
  return MultiIndex::setLogger(pLogger);
}
ILogger* IMultiIndex::getLogger(){
  return MultiIndex::getLogger();
}

IMultiIndex::~IMultiIndex() = default;
