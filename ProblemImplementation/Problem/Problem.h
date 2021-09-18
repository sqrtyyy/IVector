//
// Created by aleksei on 10.09.2021.
//

#ifndef UI_MATH_PROBLEM_H
#define UI_MATH_PROBLEM_H

#include <IDiffProblem.h>


namespace {
  class Problem : public IDiffProblem
  {
  public:
    static ILogger* m_pLogger;
    static const size_t m_argsNum;
    static const size_t m_paramsNum;
  private:
    IVector * m_params;
    IVector * m_args;
    ICompact * m_paramsCompact;
    ICompact * m_argsCompact;
  private:
    static double evaluate(const double *params, const double *args) ;
    static double evaluateDerivative(const size_t* degrees, const double *params, const double *args) ;
    static void evalGradient(IVector *const &val, const double *params, const double *args);
  public:
    Problem();
    RC setParams(const IVector *const &params) override;
    RC setArgs(const IVector *const &args) override;
    bool isValidParams(const IVector *const &params) const override;
    bool isValidArgs(const IVector *const &args) const override;
    double evalByParams(const IVector *const &params) const override;
    double evalByArgs(const IVector *const &args) const override;
    IDiffProblem * clone() const override;
    double evalDerivativeByArgs(const IVector *const &args, const IMultiIndex *const &index) const override;
    double evalDerivativeByParams(const IVector *const &params, const IMultiIndex *const &index) const override;
    RC evalGradientByArgs(const IVector *const &args, IVector *const &val) const override;
    RC evalGradientByParams(const IVector *const &params, IVector *const &val) const override;
    RC setParamsDomain(const ICompact *const &params) override;
    RC setArgsDomain(const ICompact *const &args, ILogger *logger) override;
    ~Problem() override;
  };
}


#endif //UI_MATH_PROBLEM_H
