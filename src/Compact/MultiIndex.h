//
// Created by aleksei on 22.08.2021.
//

#ifndef UI_MATH_MULTIINDEX_H
#define UI_MATH_MULTIINDEX_H

#include "IMultiIndex.h"

namespace {
  class MultiIndex : public IMultiIndex{
  private:
    unsigned m_dim;
    static ILogger* m_pLogger;
  private:
    explicit MultiIndex(unsigned dim);
    size_t* getIndices() const;
    bool isValidIndex(size_t idx, bool isLog) const;
  public:
    size_t getDim() const override;
    IMultiIndex * clone() const override;
    RC getAxisIndex(size_t axisIndex, size_t &val) const override;
    const size_t * getData() const override;
    RC incAxisIndex(size_t axisIndex, ssize_t val) override;
    RC setAxisIndex(size_t axisIndex, size_t val) override;
    RC setData(size_t dim, const size_t *const &ptr_data) override;
    static IMultiIndex * createMultiIndex(size_t dim, const size_t* indices);
    static RC setLogger(ILogger * pLogger);
    static ILogger* getLogger();
    ~MultiIndex() override;
  };
}


#endif //UI_MATH_MULTIINDEX_H
