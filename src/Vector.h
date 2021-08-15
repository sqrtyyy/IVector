//
// Created by aleksei on 3/20/21.
//

#ifndef ANTONOV_VECTOR_H
#define ANTONOV_VECTOR_H

#include <IVector.h>
namespace {
    class Vector : public IVector {
    private:
        static ILogger* m_pLogger;
        size_t m_dim;
    public:
        static Vector *createVector(size_t dim, double const *const &ptr_data);
        static RC copyInstance(IVector* dest, IVector const* const& src);
        static RC moveInstance(IVector* dest, IVector*& src);
        static RC setLogger(ILogger* logger);
        static IVector* add(IVector const* const& op1, IVector const* const& op2);
        static IVector* sub(IVector const* const& op1, IVector const* const& op2);

        static double dot(IVector const* const& op1, IVector const* const& op2);
        static bool equals(IVector const* const& op1, IVector const* const& op2, NORM n, double tol);

    public:
        RC applyFunction(const std::function<double(double)> &fun) override;

        IVector *clone() const override;

        RC dec(const IVector *const &op) override;

        RC foreach(const std::function<void(double)> &fun) const override;

        RC getCord(size_t index, double &val) const override;

        const double *getData() const override;

        size_t getDim() const override;

        RC inc(const IVector *const &op) override;

        double norm(NORM n) const override;

        RC scale(double multiplier) override;

        RC setCord(size_t index, double val) override;

        size_t sizeAllocated() const override;

        RC setData(size_t dim, const double *const &ptr_data) override;

        ~Vector() override;
    private:
        double* getRawData() const;
        explicit Vector(size_t dim);
    };
}


#endif //ANTONOV_VECTOR_H
