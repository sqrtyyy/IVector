//
// Created by aleksei on 4/3/21.
//

#ifndef ANTONOVMATH_SET_H
#define ANTONOVMATH_SET_H

#include <memory>
#include "ISet.h"
#include "ISetControlBlock.h"

namespace {
    class SetControlBlock;
    class Set : public ISet {
    private:
        double* m_pData;
        size_t* m_codes;
        size_t m_curCode;
        size_t m_size;
        size_t m_capacity;
        size_t m_dim;
        std::shared_ptr<SetControlBlock> m_pControlBlock;
    public:
        static ILogger* m_pLogger;
    private:
        RC reallocate();
        RC getIdx(const IVector *const &pat, IVector::NORM n, double tol, size_t& idx) const;
    public:
        Set();
        size_t getSize() const override;
        size_t getDim() const override;
        RC findFirstAndCopy(const IVector *const &pat,
                            IVector::NORM n,
                            double tol,
                            IVector *&val) const override;
        RC findFirstAndCopyCoords(const IVector *const &pat,
                                  IVector::NORM n,
                                  double tol,
                                  IVector *const &val) const override;
        RC getCoords(size_t index, IVector *const &val) const override;
        RC getCopy(size_t index, IVector *&val) const override;
        RC insert(const IVector *const &val, IVector::NORM n, double tol) override;
        RC remove(size_t index) override;
        RC remove(const IVector *const &pat, IVector::NORM n, double tol) override;
        ISet * clone() const override;
        RC findFirst(const IVector *const &pat, IVector::NORM n, double tol) const override;
        IIterator * getBegin() const override;
        IIterator * getEnd() const override;
        IIterator * getIterator(size_t index) const override;

    public:
        RC beginVec(IVector *vector, size_t &code) const;
        RC endVec(IVector *vector, size_t &code) const;
        RC nextVec(IVector *vector, size_t &code, size_t indexInc = 1) const;
        RC prevVec(IVector *vector, size_t &code, size_t indexDec = 1) const;
        ~Set() override;
    public:
        class Iterator : public IIterator {
        private:
            IVector* m_pVector;
            size_t m_code;
            std::shared_ptr<ISetControlBlock> m_pIteratorControlBlock;
        public:
            static ILogger* m_pIteratorLogger;
        public:
            Iterator(IVector* vec, std::shared_ptr<ISetControlBlock> p_ControlBlock, size_t code);
            IIterator * clone() const override;
            IIterator * getNext(size_t indexInc) const override;
            IIterator * getPrevious(size_t indexInc) const override;
            RC getVectorCoords(IVector *const &val) const override;
            RC getVectorCopy(IVector *&val) const override;
            RC next(size_t indexInc) override;
            RC previous(size_t indexInc) override;
            bool isValid() const override;
            RC makeBegin() override;
            RC makeEnd() override;
            ~Iterator() override;

        public:
            static RC setLogger(ILogger * pLogger);
        };
    };

    class SetControlBlock : public ISetControlBlock {
    private:
        const Set* m_pSet;
    public:
        explicit SetControlBlock(const Set * pSet);
        RC getBegin(IVector *const &vec, size_t &index) const override;
        RC getEnd(IVector *const &vec, size_t &index) const override;
        RC getNext(IVector *const &vec, size_t &index, size_t indexInc) const override;
        RC getPrevious(IVector *const &vec, size_t &index, size_t indexInc) const override;
        void deleteSet();
    };
}


#endif //ANTONOVMATH_SET_H
