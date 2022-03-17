#include "echo_detector/normalized_covariance_estimator.h"

#include "gtest/gtest.h"

namespace aqa {

TEST(NormalizedCovarianceEstimatorTests, IdenticalSignalTest) {
  NormalizedCovarianceEstimator test_estimator;
  for (size_t i = 0; i < 10000; ++i) {
    test_estimator.Update(1.f, 0.f, 1.f, 1.f, 0.f, 1.f);
    test_estimator.Update(-1.f, 0.f, 1.f, -1.f, 0.f, 1.f);
  }

  EXPECT_NEAR(1.f, test_estimator.normalized_cross_correlation(), 0.01f);
  test_estimator.Clear();
  EXPECT_EQ(0.f, test_estimator.normalized_cross_correlation());
}

TEST(NormalizedCovarianceEstimatorTests, OppositeSignalTest) {
  NormalizedCovarianceEstimator test_estimator;
  for (size_t i = 0; i < 10000; ++i) {
    test_estimator.Update(1.f, 0.f, 1.f, -1.f, 0.f, 1.f);
    test_estimator.Update(-1.f, 0.f, 1.f, 1.f, 0.f, 1.f);
  }

  EXPECT_NEAR(-1.f, test_estimator.normalized_cross_correlation(), 0.01f);
}

}  // namespace aqa
