#ifndef MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_MOVING_MAX_H_
#define MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_MOVING_MAX_H_

#include <stddef.h>

namespace aqa {

class MovingMax {
 public:
  explicit MovingMax(size_t window_size);
  ~MovingMax();

  void Update(float value);
  float max() const;

  void Clear();

 private:
  float max_value_ = 0.f;
  size_t counter_ = 0;
  size_t window_size_ = 1;
};

}  // namespace aqa


#endif  // MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_MOVING_MAX_H_
