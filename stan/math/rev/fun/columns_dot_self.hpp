#ifndef STAN_MATH_REV_FUN_COLUMNS_DOT_SELF_HPP
#define STAN_MATH_REV_FUN_COLUMNS_DOT_SELF_HPP

#include <stan/math/rev/meta.hpp>
#include <stan/math/rev/core.hpp>
#include <stan/math/rev/fun/typedefs.hpp>
#include <stan/math/rev/fun/dot_self.hpp>
#include <stan/math/prim/fun/Eigen.hpp>

namespace stan {
namespace math {

/**
 * Returns the dot product of each column of a matrix with itself.
 *
 * @tparam Mat Type of the matrix.
 * @param x Matrix.
 */
template <typename Mat, require_rev_matrix_t<Mat>* = nullptr>
inline auto columns_dot_self(const Mat& x) {
  using T_return = promote_var_matrix_t<Eigen::RowVectorXd, Mat>;

  arena_t<plain_type_t<Mat>> arena_x = x;
  arena_t<Eigen::MatrixXd> arena_x_val = value_of(arena_x);

  T_return res = (arena_x_val.array() * arena_x_val.array()).colwise().sum();

  if (x.size() > 0) {
    reverse_pass_callback([res, arena_x, arena_x_val]() mutable {
      Eigen::RowVectorXd rv = res.adj();
      arena_x.adj() += 2 * arena_x_val * rv.asDiagonal();
    });
  }

  return res;
}

}  // namespace math
}  // namespace stan
#endif
