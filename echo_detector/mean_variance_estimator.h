#ifndef MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_MEAN_VARIANCE_ESTIMATOR_H_
#define MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_MEAN_VARIANCE_ESTIMATOR_H_

namespace aqa {


// This class iteratively estimates the mean and variance of a signal.
// smooth out short-term fluctuations
// highlight longer-term trends or cycles
class MeanVarianceEstimator {
 public:
  void Update(float value);
  float std_deviation() const;
  float mean() const;
  void Clear();

 private:
  // Estimate of the expected value of the input values.
  float mean_ = 0.f;
  // Estimate of the variance of the input value.
  float variance_ = 0.f;
};

}  // namespace aqa

#endif  // MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_MEAN_VARIANCE_ESTIMATOR_H_
