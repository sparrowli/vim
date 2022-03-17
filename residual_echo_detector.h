#ifndef MODUILES_AUDIO_PROCESSING_RESIDUAL_ECHO_DETECTOR_H_
#define MODUILES_AUDIO_PROCESSING_RESIDUAL_ECHO_DETECTOR_H_

#include <vector>

#include "echo_detector/circular_buffer.h"
#include "echo_detector/mean_variance_estimator.h"
#include "echo_detector/moving_max.h"
#include "echo_detector/normalized_covariance_estimator.h"
#include "common_audio/rtc_histogram.h"
#include "audio_processing.h"

namespace aqa {

class ApmDataDumper;
// class AudioBuffer;

class ResidualEchoDetector : public EchoDetector {
 public:
  ResidualEchoDetector();
  ~ResidualEchoDetector() override;

  // should be callled while holding the render lock
  void AnalyzeRenderAudio(std::vector<float> render_audio) override;
  // should be callled while holding the capture lock
  void AnalyzeCaptureAudio(std::vector<float> capture_audio) override;

  // should be callled while holding the capture lock
  void Initialize(int capture_sample_rate_hz,
		  int num_capture_channles,
                  int render_sample_rate_hz,
		  int num_render_channles) override;

  // This function is for testing purposes only.
  void SetReliabilityForTest(float value) { reliability_ = value; }

  // should be callled while holding the capture lock
  EchoDetector::Metrics GetMetrics() const override;

 private:
  static int instance_count_;
  std::unique_ptr<ApmDataDumper> data_dumper_;
  // Keep track if the `Process` function has been previously called.
  bool first_process_call_ = true;

  // Buffer for storing the power of incoming farend buffers.
  // This is needed for cases where calls to BufferFarend and Process are jittery.
  CircularBuffer render_buffer_;

  // Count how long ago it was that the size of `render_buffer_` was zero.
  // This value is also reset to zero when clock drift is detected and a value 
  // from the render buffer is discarded, even though the buffer is not 
  // actually zero at that point. This is done to avoid repeatedly removing elements in this situation.
  size_t frames_since_zero_buffer_size_ = 0;

  // Circular buffers containing delayed versions of the power, mean and
  // standard deviation, for calculating the delayed covariance values.
  std::vector<float> render_power_;
  std::vector<float> render_power_mean_;
  std::vector<float> render_power_std_dev_;

  // Covariance estimates for different delay values.
  std::vector<NormalizedCovarianceEstimator> covariances_;

  // Index where next element should be inserted in all of the above
  // circular bufferss.
  size_t next_insertion_index_ = 0;
  
  MeanVarianceEstimator render_statistics_;
  MeanVarianceEstimator capture_statistics_;

  // Current echo likelihood.
  float echo_likelihood_ = 0.f;
  // Reliability of the current likelihood
  float reliability_ = 0.f;

  /*
     The estimated likelihood percentage of echo as detected by the residual echo
    detector. The residual echo detector can be used to detect cases where the
    AEC (hardware or software) is not functioning properly. The detector can be
    non-causal and operates on larger timescales with more delay than the
    regular AEC.
  */
  RtcHistogram echo_likelihood_histogram_;
  MovingMax recent_likelihood_max_;

  FILE* likelihood_csv_;
  FILE* likelihood_m_;

  int log_counter_ = 0;
};

}  // namespace aqa
#endif  // MODUILES_AUDIO_PROCESSING_RESIDUAL_ECHO_DETECTOR_H_
