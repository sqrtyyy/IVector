//
// Created by aleksei on 22.08.2021.
//

#ifndef UI_MATH_COMPACT_H
#define UI_MATH_COMPACT_H

#include <memory>
#include <ICompactControlBlock.h>
#include <ICompact.h>

namespace {
  class CompactControlBlock;
  class Compact : public ICompact {
  private:
    unsigned m_dim;
    static ILogger* m_pLogger;
  private:
    explicit Compact(unsigned dim);
    bool isValidMultiIdx(const IMultiIndex *index) const;
    double* getLeftData() const;
    double* getRightData() const;
    size_t* getGridRawData() const;
  public:
    ICompact * clone() const override;
    IIterator * getBegin(const IMultiIndex *const &bypassOrder) const override;
    size_t getDim() const override;
    IIterator * getEnd(const IMultiIndex *const &bypassOrder) const override;
    IMultiIndex * getGrid() const override;
    const size_t* getGridData() const;
    IIterator * getIterator(const IMultiIndex *const &index, const IMultiIndex *const &bypassOrder) const override;
    RC getLeftBoundary(IVector *&vec) const override;
    RC getRightBoundary(IVector *&vec) const override;
    RC getVectorCoords(const IMultiIndex *index, IVector *const &val) const override;
    RC getVectorCopy(const IMultiIndex *index, IVector *&val) const override;
    bool isInside(const IVector *const &vec) const override;

  public:
    static ICompact* createIntersection(ICompact const *op1, ICompact const *op2, IMultiIndex const* grid, double tol);
    static ICompact* createCompactSpan(ICompact const *op1, ICompact const *op2, IMultiIndex const* grid);
    static ICompact* createCompact(IVector const * vec1, IVector const * vec2, IMultiIndex const *nodeQuantities);

    ~Compact() override;

    class Iterator : public ICompact::IIterator {
    private:
      std::shared_ptr<CompactControlBlock> m_controlBlock;
      IMultiIndex* m_bypassOrder;
      IMultiIndex* m_curIdx;
    public:
      static ILogger* m_pIteratorLogger;
    public:
      Iterator(std::shared_ptr<CompactControlBlock> controlBlock, IMultiIndex *idx, IMultiIndex *bypassOrder);
      IIterator * clone() const override;
      IIterator * getNext() override;
      RC getVectorCoords(IVector *const &val) const override;
      RC getVectorCopy(IVector *&val) const override;
      bool isValid() const override;
      RC next() override;
      ~Iterator() override;
    };
  };

  class CompactControlBlock : public ICompactControlBlock {
  private:
    Compact* m_pCompact;
  private:
  public:
    explicit CompactControlBlock(Compact* compact);
    RC get(IMultiIndex *const &currentIndex, const IMultiIndex *const &bypassOrder) const override;
    RC get(const IMultiIndex *const &currentIndex, IVector *const &val) const override;
    RC getCopy(const IMultiIndex *const &currentIndex, IVector * &val) const;
    bool hasNext(const IMultiIndex *const &currentIndex, const IMultiIndex *const &bypassOrder) const;
  };

}
#endif //UI_MATH_COMPACT_H
