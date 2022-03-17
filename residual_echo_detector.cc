
#include "residual_echo_detector.h"


#include <numeric>
#include <iostream>

#include "logging/apm_data_dumper.h"

namespace rtc {
class AtomicOps {
 public:
  static int Increment(volatile int* i) { return __sync_add_and_fetch(i, 1); }

};
}  // namespace rtc

namespace {

float Power(std::vector<float> input) {
  if (input.empty()) {
    return 0.f;
  }

  return std::inner_product(input.begin(), input.end(), input.begin(), 0.f) /
         input.size();
}

constexpr size_t kLookbackFrames = 650;
// ToDo(lifan): Verify the size of this buffer?
constexpr size_t kRenderBufferSize = 30;
constexpr float kAlpha = 0.001f;
// 10 seconds of data, updated every 10ms.
constexpr size_t kAggregationBufferSize = 10 * 100;

}  // namespace

namespace aqa {

int ResidualEchoDetector::instance_count_ = 0;

ResidualEchoDetector::ResidualEchoDetector()
    : data_dumper_(
        new ApmDataDumper(rtc::AtomicOps::Increment(&instance_count_))),
      render_buffer_(kRenderBufferSize),
      render_power_(kLookbackFrames),
      render_power_mean_(kLookbackFrames),
      render_power_std_dev_(kLookbackFrames),
      covariances_(kLookbackFrames),
      echo_likelihood_histogram_("WebRTC.Audio.ResidualEchoDetector.EchoLikelihood",
                       0, 100, 100 /* number of bins */),
      recent_likelihood_max_(kAggregationBufferSize) {
#if RTC_METRICS_ENABLED
#endif  // RTC_METRICS_ENABLED
  likelihood_csv_ = fopen("echo_likelihood.csv", "w");
  likelihood_m_ = fopen("echo_likelihood.dat", "wb");
}

// ResidualEchoDetector::~ResidualEchoDetector() = default;
ResidualEchoDetector::~ResidualEchoDetector() {
  std::map<int, int> residual_echo_likelihood = echo_likelihood_histogram_.Samples();

  for (const auto& it : residual_echo_likelihood) {
    fprintf(likelihood_csv_, "%d\n", it.second);
    fwrite(&it.second, sizeof(int), 1, likelihood_m_);
  }

  fprintf(likelihood_csv_, "%lu\n", residual_echo_likelihood.size());
  fclose(likelihood_csv_);
  fclose(likelihood_m_);
}

void ResidualEchoDetector::AnalyzeRenderAudio(
    std::vector<float> render_audio) {
  // Todo(lifan): Dump debug data
  data_dumper_->DumpRaw("ed_render_16k_mono.pcm", render_audio.size(), &render_audio[0]); 
  if (render_buffer_.Size() == 0) {
    frames_since_zero_buffer_size_ = 0;
  } else if (frames_since_zero_buffer_size_ >= kRenderBufferSize) {
    render_buffer_.Pop();
    frames_since_zero_buffer_size_ = 0;
  }
  ++frames_since_zero_buffer_size_;
  float power = Power(render_audio);
  render_buffer_.Push(power);
  data_dumper_->DumpRaw("ed_render_power_100hz_00.pcm", power); 
}

void ResidualEchoDetector::AnalyzeCaptureAudio(
    std::vector<float> capture_audio) {
  // Todo(lifan): Dump debug data

  data_dumper_->DumpRaw("ed_capture_16k_mono.pcm", capture_audio.size(), &capture_audio[0]); 

  if (first_process_call_) {
    // On the first process call (so the start of a call), we must
    // flush the render buffer, otherwise the render data will be delayed.
    render_buffer_.Clear();
    first_process_call_ = false;
  }

  // Get the next render value
  const std::optional<float> buffered_render_power = render_buffer_.Pop();
  if (!buffered_render_power) {
    // This can happen in a few cases: at the start of a call, due to a glitch
    // or due to clock drift. The excess capture value will be ignored.
    // ToDo(lifan): Include how often this happens in APM stats.
    return;
  }

  // Update the render statistics, and store the statistics in circular buffers.
  render_statistics_.Update(*buffered_render_power);
  // check next_insertion_index_ < kLookbackFrames
  render_power_[next_insertion_index_] = *buffered_render_power;
  data_dumper_->DumpRaw("ed_render_power_100hz.pcm", render_power_[next_insertion_index_]); 
  render_power_mean_[next_insertion_index_] = render_statistics_.mean();
  data_dumper_->DumpRaw("ed_render_power_mean_100hz.pcm", render_power_mean_[next_insertion_index_]); 
  render_power_std_dev_[next_insertion_index_] =
      render_statistics_.std_deviation();
  data_dumper_->DumpRaw("ed_render_power_std_dev_100hz.pcm", render_power_std_dev_[next_insertion_index_]); 

  // Get the next capture value, update capture statistics and
  // add the relevant values to the buffers.
  const float capture_power = Power(capture_audio);
  data_dumper_->DumpRaw("ed_capture_power_100hz.pcm", capture_power); 
  capture_statistics_.Update(capture_power);
  const float capture_mean = capture_statistics_.mean();
  data_dumper_->DumpRaw("ed_capture_power_mean_100hz.pcm", capture_mean); 
  const float capture_std_deviation = capture_statistics_.std_deviation();
  data_dumper_->DumpRaw("ed_capture_power_std_dev_100hz.pcm", capture_std_deviation); 

  // Update the covariance values and determine the new echo likelihood.
  echo_likelihood_ = 0.f;
  size_t read_index = next_insertion_index_;

  int best_delay = -1;
  for (size_t delay = 0; delay < covariances_.size(); ++delay) {
    // check read_index < render_power_.size()
    covariances_[delay].Update(capture_power, capture_mean,
                               capture_std_deviation, render_power_[read_index],
			       render_power_mean_[read_index],
			       render_power_std_dev_[read_index]);
    read_index = read_index > 0 ? read_index - 1 : kLookbackFrames - 1;


    if (covariances_[delay].normalized_cross_correlation() > echo_likelihood_ ) {
      echo_likelihood_ = covariances_[delay].normalized_cross_correlation();
      best_delay = static_cast<int>(delay);
    }
  }

  if (echo_likelihood_ > 1.1f) {
    if (log_counter_ < 5 && best_delay != -1) {
      size_t read_index = kLookbackFrames + next_insertion_index_ - best_delay;
      if (read_index >= kLookbackFrames) {
        read_index -= kLookbackFrames;
      }
      // check read_index < render_power_size()
      std::cout << "Echo detector internal state: {"
	           "\nEcho likelihood:"
		<< echo_likelihood_ << "\n, Best Delay: "
		<< best_delay << "\n, Covariance: "
		<< covariances_[best_delay].covariance()
		<< "\n, Last capture power: " << capture_power
		<< "\n, Capture mean: " << capture_mean
		<< "\n, Capture_standard deviation: "
		<< capture_std_deviation
		<< "\n, Last render power: " << render_power_[read_index]
		<< "\n, Render mean: " << render_power_mean_[read_index]
		<< "\n, Render standard deviation: "
		<< render_power_std_dev_[read_index]
		<< "\n, Reliability: " << reliability_ << "\n}";
      log_counter_++;
    }
  }

  // ToDo(lifan): check echo_likelihood_ < 1.1f

  reliability_ = (1.0f - kAlpha) * reliability_ + kAlpha * 1.0f;
  echo_likelihood_ *= reliability_;

  echo_likelihood_ = std::min(echo_likelihood_, 1.0f);
  int echo_percentage = static_cast<int>(echo_likelihood_ * 100);
  // RTC_HISTOGRAM_COUNTS("WebRTC.Audio.ResidualEchoDetector.EchoLikelihood", echo_percentage, 0, 100, 100/* number of bins */);
  echo_likelihood_histogram_.Add(echo_percentage);


  // Update the buffer of recent likelihood values.
  recent_likelihood_max_.Update(echo_likelihood_);

  // Update the next insertion index.
  next_insertion_index_ = next_insertion_index_ < (kLookbackFrames - 1)
	                      ? next_insertion_index_ + 1
			      : 0;
}

void ResidualEchoDetector::Initialize(int /*capture_sample_rate_hz*/,
		                      int /*num_capture_channels*/,
		                      int /*render_sample_rate_hz*/,
		                      int /*num_render_channels*/) {
  render_buffer_.Clear();
  std::fill(render_power_.begin(), render_power_.end(), 0.f);
  std::fill(render_power_mean_.begin(), render_power_mean_.end(), 0.f);
  std::fill(render_power_std_dev_.begin(), render_power_std_dev_.end(), 0.f);

  render_statistics_.Clear();
  capture_statistics_.Clear();
  recent_likelihood_max_.Clear();

  for (auto& cov : covariances_) {
    cov.Clear();
  }

  echo_likelihood_ = 0.f;
  next_insertion_index_ = 0;
  reliability_ = 0.f;
}

EchoDetector::Metrics ResidualEchoDetector::GetMetrics() const {
  EchoDetector::Metrics metrics;
  metrics.echo_likelihood = echo_likelihood_;
  metrics.echo_likelihood_recent_max = recent_likelihood_max_.max();

  return metrics;
}


}  // namespace aqa
