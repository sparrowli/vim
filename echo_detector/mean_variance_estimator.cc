#include "echo_detector/mean_variance_estimator.h"

#include <math.h>

namespace aqa {
namespace {
// Parameter controlling the adaptation speed
// set the parameter of the exponential moving average to
// decide the threshold between short-term and long-term
// first-order IIR low-pass filter
constexpr float kAlpha = 0.001f;
}  // namespace

void MeanVarianceEstimator::Update(float value) {
  mean_ = (1.f - kAlpha) * mean_ + kAlpha * value;
  variance_ = 
      (1.f - kAlpha) * variance_ + kAlpha * (value - mean_) * (value - mean_);
  // ToDo debug check
  if ( !isfinite(mean_) ) {
  }

  if ( !isfinite(variance_) ) {
  }
}

float MeanVarianceEstimator::std_deviation() const {
  // ToDo: Check variance_ >= 0.f
  return sqrtf(variance_);
}

float MeanVarianceEstimator::mean() const {
  return mean_;
}

void MeanVarianceEstimator::Clear() {
  mean_ = 0.f;
  variance_ = 0.f;
}

}  // namespace aqa
