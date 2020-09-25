#ifndef STAN_MATH_REV_FUN_MULTIPLY_LOWER_TRI_SELF_TRANSPOSE_HPP
#define STAN_MATH_REV_FUN_MULTIPLY_LOWER_TRI_SELF_TRANSPOSE_HPP

#include <stan/math/rev/meta.hpp>
#include <stan/math/rev/core.hpp>
#include <stan/math/rev/fun/dot_product.hpp>
#include <stan/math/rev/fun/dot_self.hpp>
#include <stan/math/rev/fun/typedefs.hpp>
#include <stan/math/prim/fun/Eigen.hpp>

namespace stan {
namespace math {

template <typename T, require_rev_matrix_t<T>* = nullptr>
inline plain_type_t<T> multiply_lower_tri_self_transpose(const T& L) {
  using T_double = Eigen::MatrixXd;
  using T_var = plain_type_t<T>;

  if (L.size() == 0)
    return L;

  arena_t<T_var> arena_L = L;
  arena_t<T_double> arena_L_val
      = value_of(arena_L).template triangularView<Eigen::Lower>();

  arena_t<T_var> res = arena_L_val.template triangularView<Eigen::Lower>()
                       * arena_L_val.transpose();

  reverse_pass_callback([res, arena_L, arena_L_val]() mutable {
    const auto& adj = to_ref(res.adj());
    Eigen::MatrixXd adjL = (adj.transpose() + adj) * arena_L_val;

    for (size_t j = 1; j < adjL.cols(); ++j)
      for (size_t i = 0; i < std::min(static_cast<size_t>(adjL.rows()), j); ++i)
        adjL(i, j) = 0.0;

    arena_L.adj() += adjL;
  });

  return res;
}

}  // namespace math
}  // namespace stan
#endif
