#ifndef COMMON_AUDIO_INCLUDE_AUDIO_UTIL_H_
#define COMMON_AUDIO_INCLUDE_AUDIO_UTIL_H_

#include <stdint.h>

#include <algorithm>
#include <cmath>
#include <limits>

namespace aqa {

typedef std::numeric_limits<int16_t> limits_int16;

// The conversion functions use the following naming convention:
// S16:      int16_t [-32768, 32767]
// Float:    float   [-1.0, 1.0]
// FloatS16: float   [-32768.0, 32768.0]
// Dbfs: float [-20.0*log(10, 32768), 0] = [-90.3, 0]
// The ratio conversion functions use this naming convention:
// Ratio: float (0, +inf)
// Db: float (-inf, +inf)
static inline float S16ToFloat(int16_t v) {
  constexpr float kScaling = 1.f / 32768.f;
  return v * kScaling;
}

static inline int16_t FloatS16ToS16(float v) {
  v = std::min(v, 32767.f);
  v = std::max(v, -32768.f);
  return static_cast<int16_t>(v + std::copysign(0.5f, v));
}

static inline int16_t FloatToS16(float v) {
  v *= 32768.f;
  v = std::min(v, 32767.f);
  v = std::max(v, -32768.f);

  return static_cast<int16_t>(v + std::copysign(0.5f, v));
}

static inline float FloatToFloatS16(float v) {
  v = std::min(v, 1.f);
  v = std::max(v, -1.f);
  return v * 32768.f;
}

static inline float FloatS16ToFloat(float v) {
  v = std::min(v, 32768.f);
  v = std::max(v, -32768.f);
  constexpr float kScaling = 1.f / 32768.f;
  return v * kScaling;
}

void FloatToS16(const float* src, size_t size, int16_t* dest);
void S16ToFloat(const int16_t* src, size_t size, float* dest);
void S16ToFloatS16(const int16_t* src, size_t size, float* dest);
void FloatS16ToS16(const float* src, size_t size, int16_t* dest);
void FloatToFloatS16(const float* src, size_t size, int16_t* dest);
void FloatS16ToFloat(const float* src, size_t size, int16_t* dest);

inline float DbToRatio(float v) {
  return std::pow(10.0f, v / 20.0f);
}

inline float DbfsToFloatS16(float v) {
  static constexpr float kMaximumAbsFloatS16 = -limits_int16::min();
  return DbToRatio(v) * kMaximumAbsFloatS16; 
}

inline float FloatS16ToDbfs(float v) {
  // ToDo(lifan): check v >= 0

  static constexpr float kMinDbfs = -90.30899869919436f;
  if (v <= 1.0f) {
    return kMinDbfs;
  } 

  // Equal to 20 * log10(v / (-limits_int16::min()))
  return 20.0f * std::log10(v) + kMinDbfs;
}

}  // namespace aqa
#endif  // COMMON_AUDIO_INCLUDE_AUDIO_UTIL_H_
