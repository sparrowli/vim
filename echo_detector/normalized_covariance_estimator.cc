#include "echo_detector/normalized_covariance_estimator.h"

namespace aqa {

namespace {
  constexpr float kAlpha = 0.001f;
}  // namespace

void NormalizedCovarianceEstimator::Update(float x,
                                           float x_mean,
                                           float x_sigma,
                                           float y,
                                           float y_mean,
                                           float y_sigma) {
  covariance_ =
      (1.f - kAlpha) * covariance_ + kAlpha * (x - x_mean) * (y - y_mean);

  normalized_cross_correlation_ = covariance_ / (x_sigma * y_sigma + .0001f);

  // ToDo check covariance_ && normalized_cross_correlation is finite
}

void NormalizedCovarianceEstimator::Clear() {
  normalized_cross_correlation_ = 0.f;
  covariance_ = 0.f;
}

}  // namespace aqa
