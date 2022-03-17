#include "echo_detector/circular_buffer.h"

namespace aqa {

CircularBuffer::CircularBuffer(size_t size) : buffer_(size) {}
CircularBuffer::~CircularBuffer() = default;


void CircularBuffer::Push(float value) {
  buffer_[next_insertion_index_] = value;
  ++next_insertion_index_;
  next_insertion_index_ %= buffer_.size();
  // index < buffer_.size();

  // ToDo: Record the distribution of discard data
  nr_elements_in_buffer_ = std::min(++nr_elements_in_buffer_, buffer_.size());
  // nr_elements_in_buffer_ <= buffer_.size();
}

std::optional<float> CircularBuffer::Pop() {
  if (0 == nr_elements_in_buffer_) {
    return std::nullopt;
  }

  const size_t index = 
      (buffer_.size() + next_insertion_index_ - nr_elements_in_buffer_) %
      buffer_.size();

  // index < buffer_.size();
  --nr_elements_in_buffer_;
  return buffer_[index];
}

void CircularBuffer::Clear() {
  std::fill(buffer_.begin(), buffer_.end(), 0.f);
  next_insertion_index_ = 0;
  nr_elements_in_buffer_ = 0;
}

}  // namespace aqa
