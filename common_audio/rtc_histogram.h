#ifndef COMMON_AUDIO_RTC_HISTOGRAM_H_
#define COMMON_AUDIO_RTC_HISTOGRAM_H_

#include <string>
#include <map>
#include <memory>

namespace aqa {

// Limit for the maximum number of sample values that can be stored.
// TODO(asapersson): Consider using bucket count (and set up
// linearly/exponentially spaced buckets) if samples are logged more frequently.
const int kMaxSampleMapSize = 300;

struct SampleInfo {
  SampleInfo(const std::string& name, int min, int max, size_t bucket_count);
  ~SampleInfo();

  const std::string name;
  const int min;
  const int max;
  const size_t bucket_count;
  std::map<int, int> samples;
};

class RtcHistogram {
 public:
  RtcHistogram(const std::string& name, int min, int max, int bucket_count);

  ~RtcHistogram() = default;

  void Add(int sample);
  std::unique_ptr<SampleInfo> GetAndReset();
  const std::string& name() const { return info_.name; }
  void Reset();
  int NumEvents(int sample) const;
  int NumSamples() const;
  int MinSample() const;
  std::map<int, int> Samples() const;

 private:
  const int min_;
  const int max_;
  SampleInfo info_;
};

}  // namespace aqa

#endif  // COMMON_AUDIO_RTC_HISTOGRAM_H_
