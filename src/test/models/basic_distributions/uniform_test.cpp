#include <gtest/gtest.h>
#include <test/models/model_test_fixture.hpp>

class Models_BasicDistributions_Uniform :
  public Model_Test_Fixture<Models_BasicDistributions_Uniform> {
protected:
  virtual void SetUp() {
  }
public:
  static std::vector<std::string> get_model_path() {
    std::vector<std::string> model_path;
    model_path.push_back("models");
    model_path.push_back("basic_distributions");
    model_path.push_back("uniform");
    return model_path;
  }
  static bool has_data() {
    return false;
  }

  static size_t num_iterations() {
    return iterations;
  }

  static void populate_chains() {
    default_populate_chains();
  }

  static std::vector<std::pair<size_t, double> >
  get_expected_values() {
    using std::make_pair;
    std::vector<std::pair<size_t, double> > expected_values;

    expected_values.push_back(make_pair(0U, 0.5));

    return expected_values;
  }

};

INSTANTIATE_TYPED_TEST_CASE_P(Models_BasicDistributions_Uniform,
			      Model_Test_Fixture,
			      Models_BasicDistributions_Uniform);

TEST_F(Models_BasicDistributions_Uniform,
       Test_Variance) {
  populate_chains();

  double sd = chains->sd(0U);
  double neff = chains->effective_sample_size(0U);
  
  EXPECT_NEAR(0.28867, sd, 1.96 * 1 / std::sqrt(2 * (neff - 1)));
}
