#include "echo_detector/moving_max.h"

namespace aqa {
namespace {
constexpr float kDecayFactor = 0.99f;
}  // namespace

MovingMax::MovingMax(size_t window_size) : window_size_(window_size) {
  // ToDo debug check window_size > 0
}

MovingMax::~MovingMax() {}

void MovingMax::Update(float value) {
  if (counter_ >= window_size_ -1) {
    max_value_ *= kDecayFactor;
  } else {
    ++counter_;
  }

  if (value > max_value_) {
    max_value_ = value;
    counter_ = 0;
  }
}

float MovingMax::max() const {
  return max_value_;
}

void MovingMax::Clear() {
  max_value_ = 0.f;
  counter_ = 0;
}

}  // namespace aqa
