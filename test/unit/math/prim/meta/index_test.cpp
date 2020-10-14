#include <type_traits>
#include <vector>
#include <stan/math.hpp>
#include <gtest/gtest.h>

TEST(MathIndexingIndex, index_uni) {
  using stan::math::index_uni;
  index_uni idx(17);
  EXPECT_EQ(17, idx.n_);
}

TEST(MathIndexingIndex, index_multi) {
  using stan::math::index_multi;
  std::vector<int> ns;
  ns.push_back(3);
  ns.push_back(23);

  index_multi idx(ns);
  EXPECT_EQ(2, idx.ns_.size());
  for (size_t i = 0; i < ns.size(); ++i)
    EXPECT_EQ(ns[i], idx.ns_[i]);
}

TEST(MathIndexingIndex, index_omni) {
  using stan::math::index_omni;
  index_omni idx;
  (void)idx;  // just to silence compiler griping about idx being unused
}

TEST(MathIndexingIndex, index_min) {
  using stan::math::index_min;
  index_min idx(3);
  EXPECT_EQ(3, idx.min_);
}

TEST(MathIndexingIndex, index_max) {
  using stan::math::index_max;
  index_max idx(912);
  EXPECT_EQ(912, idx.max_);
}

TEST(MathIndexingIndex, index_min_max) {
  using stan::math::index_min_max;
  index_min_max idx(401, 912);
  EXPECT_EQ(401, idx.min_);
  EXPECT_EQ(912, idx.max_);
}
