#ifndef MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_CIRCULAR_BUFFER_H_
#define MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_CIRCULAR_BUFFER_H_

#include <vector>
#include <optional>

namespace aqa {
struct CircularBuffer {
 public:
  explicit CircularBuffer(size_t size);
  ~CircularBuffer();


  void Push(float value);
  std::optional<float> Pop();
  size_t Size() const { return nr_elements_in_buffer_; }
  // This function fills the buffer with zeros, but does not change its size.
  void Clear();

 private:
  std::vector<float> buffer_;
  size_t next_insertion_index_ = 0;
  // This is the number of elements that have been pushed into the circular
  // buffer, not the allocated buffer size.
  size_t nr_elements_in_buffer_ = 0;
};
}  // namespace aqa

#endif  // MODULES_AUDIO_PROCESSING_ECHO_DETECTOR_CIRCULAR_BUFFER_H_
