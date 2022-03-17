#ifndef MODULES_AUDIO_PROCESSING_INCLUDE_AUDIO_PROCESSING_H_
#define MODULES_AUDIO_PROCESSING_INCLUDE_AUDIO_PROCESSING_H_

namespace aqa {

class AudioBuffer;
// Interface for an echo detector submodule.

// ToDo ref count

class EchoDetector {

 public:
  virtual void Initialize(int capture_sample_rate_hz,
                          int num_capture_channels,
                          int render_sample_rate_hz,
			  int num_render_channels) = 0;

  virtual void AnalyzeRenderAudio(
      std::vector<float> render_audio) = 0;

  virtual void AnalyzeCaptureAudio(
      std::vector<float> capture_audio) = 0;
  
  static void PackRenderAudioBuffer(AudioBuffer* audio,
                                       std::vector<float>* packed_buffer);

  struct Metrics {
    double echo_likelihood;
    double echo_likelihood_recent_max;
  };

  virtual Metrics GetMetrics() const = 0;

 protected:
  virtual ~EchoDetector() {}
};

}  // namespace aqa

#endif  // MODULES_AUDIO_PROCESSING_INCLUDE_AUDIO_PROCESSING_H_
