#include "echo_detector/mean_variance_estimator.h"

#include "gtest/gtest.h"

namespace aqa {

TEST(MeanVarianceEstimatorTests, InsertTwoValues) {
  MeanVarianceEstimator test_estimator;

  test_estimator.Update(3.f);
  test_estimator.Update(5.f);

  EXPECT_GT(test_estimator.mean(), 0.f);
  EXPECT_GT(test_estimator.std_deviation(), 0.f);

  test_estimator.Clear();
  EXPECT_EQ(test_estimator.mean(), 0.f);
  EXPECT_EQ(test_estimator.std_deviation(), 0.f);
}

TEST(MeanVarianceEstimatorTests, InsertZeros) {
  MeanVarianceEstimator test_estimator;
 
  for (size_t i = 0; i < 20000; ++i) {
    test_estimator.Update(0.f);
  }
  EXPECT_EQ(test_estimator.mean(), 0.f);
  EXPECT_EQ(test_estimator.std_deviation(), 0.f);
}

TEST(MeanVarianceEstimatorTests, ConstantValueTest) {
  MeanVarianceEstimator test_estimator;
 
  // the mean has converged to within 1%
  // 4603 = log(0.01, 0.999) as kAlpha = 0.001, while the std_deviation converge to 10%
  // 9205 = 2 * 4603 the std_deviation converge to 1%
  for (size_t i = 0; i < 9205; ++i) {
    test_estimator.Update(1.f);
  }
  // mean_ ~= 3.0f +- 0.01f, std_deviation_ ~= 0.f +- 0.01f
  EXPECT_NEAR(1.0f, test_estimator.mean(), 0.01f);
  EXPECT_NEAR(0.0f, test_estimator.std_deviation(), 0.01f);
}

TEST(MeanVarianceEstimatorTests, ConstantValueConvergeTo1PercentTest) {
  MeanVarianceEstimator test_estimator;
 
  // the calculation has converged to within 1%
  // 4603 = log(0.01, 0.999) as kAlpha = 0.001
  for (size_t i = 0; i < 9205; ++i) {
    test_estimator.Update(3.f);
  }
  // mean_ ~= 3.0f +- 0.01f, std_deviation_ ~= 0.f +- 0.01f
  EXPECT_NEAR(3.0f, test_estimator.mean(), 3.f * 0.01f);
  EXPECT_NEAR(0.0f, test_estimator.std_deviation(), 3.f * 0.01f);
}

TEST(MeanVarianceEstimatorTests, AlternatingValueTest) {
  MeanVarianceEstimator test_estimator;
 
  for (size_t i = 0; i < 9205; ++i) {
    test_estimator.Update(3.f);
    test_estimator.Update(-3.f);
  }
  // mean_ ~= 3.0f +- 0.01f, std_deviation_ ~= 0.f +- 0.01f
  EXPECT_NEAR(0.0f, test_estimator.mean(), 0.01f);
  EXPECT_NEAR(3.0f, test_estimator.std_deviation(), 0.01f);
}

}  // namespace aqa
