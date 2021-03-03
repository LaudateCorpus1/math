#ifndef STAN_MATH_OPENCL_REV_LOG_SUM_EXP_HPP
#define STAN_MATH_OPENCL_REV_LOG_SUM_EXP_HPP
#ifdef STAN_OPENCL

#include <stan/math/opencl/prim/log_sum_exp.hpp>
#include <stan/math/opencl/kernel_generator.hpp>
#include <stan/math/rev/core.hpp>
#include <stan/math/rev/fun/value_of.hpp>

namespace stan {
namespace math {

/**
 * Returns the dot product.
 *
 * @tparam T1 type of the first vector
 * @tparam T2 type of the second vector
 *
 * @param[in] v1 First vector.
 * @param[in] v2 Second vector.
 * @return Dot product of the vectors.
 * @throw std::domain_error if sizes of v1 and v2 do not match.
 */
template <typename T,
          require_all_kernel_expressions_and_none_scalar_t<T>* = nullptr>
inline var log_sum_exp(const var_value<T>& A) {
  return make_callback_var(
      log_sum_exp(A.val()),
      [A](vari& res) mutable {
        A.adj() += res.adj() * exp(A.val() - res.val());
      });
}

}  // namespace math
}  // namespace stan

#endif
#endif
