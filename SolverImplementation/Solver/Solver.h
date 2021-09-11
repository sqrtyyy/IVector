//
// Created by aleksei on 10.09.2021.
//

#ifndef UI_MATH_SOLVER_H
#define UI_MATH_SOLVER_H

#include <ISolver.h>
#include <IDiffProblem.h>

namespace {

  class Solver : public ISolver
  {
  private:
    IDiffProblem* m_problem;
    ICompact * m_params;
    ICompact * m_args;
    IVector* m_solution;
    static const unsigned m_methodParamsNum;
    static const unsigned m_maxStepsNum;
  private:
    RC solveSystemHesssian(double * const* H, double const * b, double const * x);
    RC calcHessian(IVector const* x, double * const* H) const;
    RC NewtonMethod(const IVector * x_0, const IVector *params, double tolerance);
    Solver();
  public:
    ISolver * clone() const override;
    RC getSolution(IVector *&solution) const override;
    bool isValidArgsDomain(const ICompact *const &args) const override;
    bool isValidParamsDomain(const ICompact *const &params) const override;
    RC setArgsDomain(const ICompact *const &args) override;
    RC setParamsDomain(const ICompact *const &params) override;
    RC setProblem(const IDiffProblem *const &pProblem) override;
    RC solveByArgs(const IVector *const &initArg, IVector *const *const &solverParams) override;
    RC solveByParams(const IVector *const &initParam, IVector *const *const &solverParams) override;
    ~Solver() override;
  };
}


#endif //UI_MATH_SOLVER_H
