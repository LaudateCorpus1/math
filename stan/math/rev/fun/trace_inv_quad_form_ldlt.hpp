#ifndef STAN_MATH_REV_FUN_TRACE_INV_QUAD_FORM_LDLT_HPP
#define STAN_MATH_REV_FUN_TRACE_INV_QUAD_FORM_LDLT_HPP

#include <stan/math/rev/meta.hpp>
#include <stan/math/rev/core.hpp>
#include <stan/math/rev/fun/LDLT_alloc.hpp>
#include <stan/math/rev/fun/LDLT_factor.hpp>
#include <stan/math/rev/core/typedefs.hpp>
#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/err.hpp>
#include <stan/math/prim/fun/Eigen.hpp>
#include <stan/math/prim/fun/typedefs.hpp>
#include <type_traits>

namespace stan {
namespace math {
namespace internal {

template <typename T2, int R2, int C2, typename T3, int R3, int C3>
class trace_inv_quad_form_ldlt_impl : public chainable_alloc {
 protected:
  inline void initializeB(const Eigen::Matrix<var, R3, C3> &B, bool haveD) {
    matrix_d Bd = B.val();
    variB_ = B.vi();
    AinvB_ = A_.solve(Bd);
    if (haveD) {
      C_.noalias() = Bd.transpose() * AinvB_;
    } else {
      value_ = (Bd.transpose() * AinvB_).trace();
    }
  }
  inline void initializeB(const Eigen::Matrix<double, R3, C3> &B, bool haveD) {
    AinvB_ = A_.solve(B);
    if (haveD) {
      C_.noalias() = B.transpose() * AinvB_;
    } else {
      value_ = (B.transpose() * AinvB_).trace();
    }
  }

  template <int R1, int C1>
  inline void initializeD(const Eigen::Matrix<var, R1, C1> &D) {
    D_ = D.val();
    variD_ = D.vi();
  }
  template <int R1, int C1>
  inline void initializeD(const Eigen::Matrix<double, R1, C1> &D) {
    D_ = D;
  }

 public:
  template <typename T1, int R1, int C1>
  trace_inv_quad_form_ldlt_impl(const Eigen::Matrix<T1, R1, C1> &D,
                                const LDLT_factor<Eigen::Matrix<T2, R2, C2>> &A,
                                const Eigen::Matrix<T3, R3, C3> &B)
      : Dtype_(stan::is_var<T1>::value), A_(A) {
    initializeB(B, true);
    initializeD(D);

    value_ = (D_ * C_).trace();
  }

  trace_inv_quad_form_ldlt_impl(const LDLT_factor<Eigen::Matrix<T2, R2, C2>> &A,
                                const Eigen::Matrix<T3, R3, C3> &B)
      : Dtype_(2), A_(A) {
    initializeB(B, false);
  }

  const int Dtype_;  // 0 = double, 1 = var, 2 = missing
  LDLT_factor<Eigen::Matrix<T2, R2, C2>> A_;
  matrix_d D_;
  matrix_vi variD_;
  matrix_vi variB_;
  matrix_d AinvB_;
  matrix_d C_;
  double value_;
};

template <typename T2, int R2, int C2, typename T3, int R3, int C3>
class trace_inv_quad_form_ldlt_vari : public vari {
 protected:
  static inline void chainA(
      double adj,
      trace_inv_quad_form_ldlt_impl<double, R2, C2, T3, R3, C3> *impl) {}
  static inline void chainB(
      double adj,
      trace_inv_quad_form_ldlt_impl<T2, R2, C2, double, R3, C3> *impl) {}

  static inline void chainA(
      double adj,
      trace_inv_quad_form_ldlt_impl<var, R2, C2, T3, R3, C3> *impl) {
    Eigen::Matrix<double, R2, C2> aA;

    if (impl->Dtype_ != 2) {
      aA.noalias()
          = -adj
            * (impl->AinvB_ * impl->D_.transpose() * impl->AinvB_.transpose());
    } else {
      aA.noalias() = -adj * (impl->AinvB_ * impl->AinvB_.transpose());
    }

    impl->A_.adj() += aA;
  }
  static inline void chainB(
      double adj,
      trace_inv_quad_form_ldlt_impl<T2, R2, C2, var, R3, C3> *impl) {
    matrix_d aB;

    if (impl->Dtype_ != 2) {
      aB.noalias() = adj * impl->AinvB_ * (impl->D_ + impl->D_.transpose());
    } else {
      aB.noalias() = 2 * adj * impl->AinvB_;
    }

    impl->variB_.adj() += aB;
  }

 public:
  explicit trace_inv_quad_form_ldlt_vari(
      trace_inv_quad_form_ldlt_impl<T2, R2, C2, T3, R3, C3> *impl)
      : vari(impl->value_), impl_(impl) {}

  virtual void chain() {
    // F = trace(D * B' * inv(A) * B)
    // aA = -aF * inv(A') * B * D' * B' * inv(A')
    // aB = aF*(inv(A) * B * D + inv(A') * B * D')
    // aD = aF*(B' * inv(A) * B)
    chainA(adj_, impl_);

    chainB(adj_, impl_);

    if (impl_->Dtype_ == 1) {
      impl_->variD_.adj() += adj_ * impl_->C_;
    }
  }

  trace_inv_quad_form_ldlt_impl<T2, R2, C2, T3, R3, C3> *impl_;
};

}  // namespace internal

/**
 * Compute the trace of an inverse quadratic form.  I.E., this computes
 *       trace(B^T A^-1 B)
 * where the LDLT_factor of A is provided.
 *
 * @tparam T type of elements in the LDLT_factor
 * @tparam R number of rows, can be Eigen::Dynamic
 * @tparam C number of columns, can be Eigen::Dynamic
 * @tparam EigMat3 type of the second matrix
 *
 * @param A an LDLT_factor
 * @param B a matrix
 * @return The trace of the inverse quadratic form.
 */
template <typename T, int R, int C, typename EigMat,
          typename = require_any_st_var<T, EigMat>>
inline return_type_t<T, EigMat> trace_inv_quad_form_ldlt(
    const LDLT_factor<Eigen::Matrix<T, R, C>> &A, const EigMat &B) {
  using T2 = value_type_t<EigMat>;
  constexpr int R2 = EigMat::RowsAtCompileTime;
  constexpr int C2 = EigMat::ColsAtCompileTime;
  check_multiplicable("trace_inv_quad_form_ldlt", "A", A, "B", B);
  if (A.cols() == 0) {
    return 0;
  }

  auto *impl_
      = new internal::trace_inv_quad_form_ldlt_impl<T, R, C, T2, R2, C2>(A, B);

  return var(
      new internal::trace_inv_quad_form_ldlt_vari<T, R, C, T2, R2, C2>(impl_));
}

}  // namespace math
}  // namespace stan
#endif
