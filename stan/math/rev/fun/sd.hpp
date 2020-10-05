#ifndef STAN_MATH_REV_FUN_SD_HPP
#define STAN_MATH_REV_FUN_SD_HPP

#include <stan/math/rev/meta.hpp>
#include <stan/math/rev/core.hpp>
#include <stan/math/rev/functor/reverse_pass_callback.hpp>
#include <stan/math/rev/core/arena_matrix.hpp>
#include <stan/math/prim/err.hpp>
#include <stan/math/prim/fun/Eigen.hpp>
#include <stan/math/prim/fun/typedefs.hpp>
#include <stan/math/prim/fun/inv_sqrt.hpp>
#include <cmath>
#include <vector>

namespace stan {
namespace math {

/**
 * Return the sample standard deviation of the specified std vector, column
 * vector, row vector, or matrix.
 *
 * @tparam R number of rows, can be Eigen::Dynamic
 * @tparam C number of columns, can be Eigen::Dynamic
 * @param[in] m input matrix
 * @return sample standard deviation of specified matrix
 * @throw domain error  size is not greater than zero.
 */
template <typename T, require_container_vt<is_var, T>* = nullptr>
var sd(const T& m) {
  check_nonzero_size("sd", "m", m);

  if (m.size() == 1) {
    return 0;
  }

  const auto& m_ref = to_ref(m);

  return apply_vector_unary<decltype(m_ref)>::reduce(m_ref, [](const auto& x) {
    auto arena_x = to_arena(x);
    const auto& x_val = to_ref(value_of(arena_x));

    auto arena_diff = to_arena((x_val.array() - x_val.mean()).matrix());
    double sum_of_squares = arena_diff.squaredNorm();
    double sd = std::sqrt(sum_of_squares / (x.size() - 1));

    var res = sd;

    reverse_pass_callback([arena_x, res, arena_diff]() mutable {
      arena_x.adj()
          += (res.adj() / (res.val() * (arena_x.size() - 1))) * arena_diff;
    });

    return res;
  });
}

}  // namespace math
}  // namespace stan
#endif
