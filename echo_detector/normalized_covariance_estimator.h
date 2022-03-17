
#ifndef MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_NORMALIZED_COVARIANCE_ESTIMATOR_H_
#define MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_NORMALIZED_COVARIANCE_ESTIMATOR_H_

namespace aqa{

class NormalizedCovarianceEstimator {

 public:
  void Update(float x,
               float x_mean,
	       float x_var,
	       float y,
	       float y_mean,
	       float y_var);

  // returns an estimation of the Pearson product-moment correlation
  // coefficient of the two signals.
  float normalized_cross_correlation() const {
    return normalized_cross_correlation_;
  }

  float covariance() const { return covariance_; }

  void Clear();

 private:
  float normalized_cross_correlation_ = 0.f;
  float covariance_ = 0.f;
};

}  // namespace aqa


#endif  // MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_NORMALIZED_COVARIANCE_ESTIMATOR_H_
