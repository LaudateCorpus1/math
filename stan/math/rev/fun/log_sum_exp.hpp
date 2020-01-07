#ifndef STAN_MATH_REV_SCAL_FUN_LOG_SUM_EXP_HPP
#define STAN_MATH_REV_SCAL_FUN_LOG_SUM_EXP_HPP

#include <stan/math/rev/meta.hpp>
#include <stan/math/rev/core.hpp>
#include <stan/math/rev/scal/fun/calculate_chain.hpp>
#include <stan/math/prim/scal/fun/constants.hpp>
#include <stan/math/prim/scal/fun/log_sum_exp.hpp>

namespace stan {
namespace math {

namespace internal {

class log_sum_exp_vv_vari : public op_vv_vari {
 public:
  log_sum_exp_vv_vari(vari* avi, vari* bvi)
      : op_vv_vari(log_sum_exp(avi->val_, bvi->val_), avi, bvi) {}
  void chain() {
    avi_->adj_ += adj_ * calculate_chain(avi_->val_, val_);
    bvi_->adj_ += adj_ * calculate_chain(bvi_->val_, val_);
  }
};
class log_sum_exp_vd_vari : public op_vd_vari {
 public:
  log_sum_exp_vd_vari(vari* avi, double b)
      : op_vd_vari(log_sum_exp(avi->val_, b), avi, b) {}
  void chain() {
    if (val_ == NEGATIVE_INFTY) {
      avi_->adj_ += adj_;
    } else {
      avi_->adj_ += adj_ * calculate_chain(avi_->val_, val_);
    }
  }
};
class log_sum_exp_dv_vari : public op_dv_vari {
 public:
  log_sum_exp_dv_vari(double a, vari* bvi)
      : op_dv_vari(log_sum_exp(a, bvi->val_), a, bvi) {}
  void chain() {
    if (val_ == NEGATIVE_INFTY) {
      bvi_->adj_ += adj_;
    } else {
      bvi_->adj_ += adj_ * calculate_chain(bvi_->val_, val_);
    }
  }
};

}  // namespace internal

/**
 * Returns the log sum of exponentials.
 */
inline var log_sum_exp(const var& a, const var& b) {
  return var(new internal::log_sum_exp_vv_vari(a.vi_, b.vi_));
}
/**
 * Returns the log sum of exponentials.
 */
inline var log_sum_exp(const var& a, double b) {
  return var(new internal::log_sum_exp_vd_vari(a.vi_, b));
}
/**
 * Returns the log sum of exponentials.
 */
inline var log_sum_exp(double a, const var& b) {
  return var(new internal::log_sum_exp_dv_vari(a, b.vi_));
}

}  // namespace math
}  // namespace stan
#endif
#ifndef STAN_MATH_REV_ARR_FUN_LOG_SUM_EXP_HPP
#define STAN_MATH_REV_ARR_FUN_LOG_SUM_EXP_HPP

#include <stan/math/rev/core.hpp>
#include <stan/math/rev/scal/fun/calculate_chain.hpp>
#include <stan/math/prim/arr/fun/log_sum_exp.hpp>
#include <stan/math/prim/scal/fun/constants.hpp>
#include <vector>
#include <cmath>

namespace stan {
namespace math {

namespace internal {
inline double log_sum_exp_as_double(const std::vector<var>& x) {
  using std::exp;
  using std::log;
  double max = NEGATIVE_INFTY;
  for (size_t i = 0; i < x.size(); ++i) {
    if (x[i] > max) {
      max = x[i].val();
    }
  }
  double sum = 0.0;
  for (size_t i = 0; i < x.size(); ++i) {
    if (x[i] != NEGATIVE_INFTY) {
      sum += exp(x[i].val() - max);
    }
  }
  return max + log(sum);
}

class log_sum_exp_vector_vari : public op_vector_vari {
 public:
  explicit log_sum_exp_vector_vari(const std::vector<var>& x)
      : op_vector_vari(log_sum_exp_as_double(x), x) {}
  void chain() {
    for (size_t i = 0; i < size_; ++i) {
      vis_[i]->adj_ += adj_ * calculate_chain(vis_[i]->val_, val_);
    }
  }
};
}  // namespace internal

/**
 * Returns the log sum of exponentials.
 */
inline var log_sum_exp(const std::vector<var>& x) {
  return var(new internal::log_sum_exp_vector_vari(x));
}

}  // namespace math
}  // namespace stan
#endif
