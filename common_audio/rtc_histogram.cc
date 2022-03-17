#include "common_audio/rtc_histogram.h"

namespace aqa {

SampleInfo::SampleInfo(const std::string& name,
                       int min,
                       int max,
                       size_t bucket_count)
    : name(name), min(min), max(max), bucket_count(bucket_count) {}


SampleInfo::~SampleInfo() {}

RtcHistogram::RtcHistogram(const std::string& name, int min, int max, int bucket_count)
    : min_(min), max_(max), info_(name, min, max, bucket_count) { /* ToDo(lifan): check bucket_count > 0*/ }

// RtcHistogram::~RtcHistogram() {}

void RtcHistogram::Add(int sample) {
  // x axis range
  sample = std::min(sample, max_);
  sample = std::max(sample, min_ - 1);  // Underflow bucket

  // ToDo(lifan): lock
  if (info_.samples.size() == kMaxSampleMapSize &&
      info_.samples.find(sample) == info_.samples.end()) {
    return;
  }

  ++info_.samples[sample];
}

std::unique_ptr<SampleInfo> RtcHistogram::GetAndReset() {
  // ToDo(lifan): lock
  if (info_.samples.empty())
    return nullptr;

  SampleInfo* copy =
      new SampleInfo(info_.name, info_.min, info_.max, info_.bucket_count);

  std::swap(info_.samples, copy->samples);

  return std::unique_ptr<SampleInfo>(copy);
}

void RtcHistogram::Reset() {
  // ToDo(lifan): lock
  info_.samples.clear();
}

// return y
int RtcHistogram::NumEvents(int sample) const {
  // ToDo(lifan): lock
  const auto it = info_.samples.find(sample);
  return (it == info_.samples.end()) ? 0 : it->second;
}

// total for normalization
int RtcHistogram::NumSamples() const {
  int num_samples = 0;
  // ToDo(lifan): lock
  for (const auto& sample : info_.samples) {
    num_samples += sample.second;
  }

  return num_samples;
}

int RtcHistogram::MinSample() const {
  // ToDo(lifan): lock
  return (info_.samples.empty()) ? -1 : info_.samples.begin()->first;
}

std::map<int, int> RtcHistogram::Samples() const {
  // ToDo(lifan): lock
  return info_.samples;
}

}  // namespace aqa
