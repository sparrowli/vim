/* ------------------------------------------------------------------
 * Copyright (C) 2022 Li Fan
 *
 * -------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdint.h>
#include <iostream>

#if defined(_MSC_VER)
#include <getopt.h>
#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include "wavreader.h"
#include "common_audio/include/audio_util.h"

#include "residual_echo_detector.h"

void usage(const char* name) {
  fprintf(stderr, "%s [-e adaptive_filter_out.wav] [-t aot] [-a afterburner] [-s sbr] [-v vbr]"
                   " mic_d.wav ref_x.wav\n", name);
  fprintf(stderr, "Supported AOTs:\n");
  fprintf(stderr, "\t2\tAAC-LC\n");
}

int main(int argc, char *argv[]) {
  int ch;
  const char *mic_d;
  const char *ref_x;
  const char *laf_e;
  void *x_wav;
  void *d_wav;
  int format, sample_rate, channels, bits_per_sample;
  int input_size;
  uint8_t* input_buf;
  int16_t* convert_buf;

  while ((ch = getopt(argc, argv, "e:t:a:s:v:")) != -1) {
    switch (ch) {
    case 'e':
            laf_e = optarg;//atoi(optarg);
            fprintf(stderr, "\tlaf_e = %s\n", laf_e);
            break;
    case 't':
            break;
    case 'a':
            break;
    case 's':
            break;
    case 'v':
            break;
    case '?':
    default:
           usage(argv[0]);
           return 1;
    }
  }

  if (argc - optind < 2) {
    fprintf(stderr, "\targc = %d\toptind = %d\targc-optind=%d\n", argc, optind, argc-optind);
    usage(argv[0]);
    return 1;
  }

  fprintf(stderr, "\targc = %d\toptind = %d\targc-optind=%d\n", argc, optind, argc-optind);
/*
  outfile = argv[optind + 1];
*/

  mic_d = argv[optind];
  ref_x = argv[optind + 1];
  fprintf(stderr, "\tmic_d = %s\tref_d=%s\n", mic_d, ref_x);

  x_wav = wav_read_open(ref_x);
  if (!x_wav) {
    fprintf(stderr, "Unable to open wav file %s\n", ref_x);
    return 1;
  }
  d_wav = wav_read_open(mic_d);
  if (!d_wav) {
    fprintf(stderr, "Unable to open wav file %s\n", mic_d);
    return 1;
  }

  if (!wav_get_header(x_wav, &format, &channels, &sample_rate, &bits_per_sample, NULL)) {
    fprintf(stderr, "Bad wav file %s\n", ref_x);
    return 1;
  }

  fprintf(stderr, "wav file: %s\n", ref_x);
  fprintf(stderr, "format: %d\n", format);
  fprintf(stderr, "channels: %d\n", channels);
  fprintf(stderr, "sample_rate: %d\n", sample_rate);
  fprintf(stderr, "bit_per_sample: %d\n", bits_per_sample);
  fprintf(stderr, "\n");

  if (format != 1) {
    fprintf(stderr, "Unsupported WAV format %d\n", format);
    return 1;
  }

  if (bits_per_sample != 16) {
    fprintf(stderr, "Unsupported WAV sample depth %d\n", bits_per_sample);
    return 1;
  }

  if (!wav_get_header(d_wav, &format, &channels, &sample_rate, &bits_per_sample, NULL)) {
    fprintf(stderr, "Bad wav file %s\n", ref_x);
    return 1;
  }

  int frame_length = 160; // 10ms for 16k

  input_size = channels*2*frame_length;
  input_buf = (uint8_t*) malloc(input_size);
  convert_buf = (int16_t*) malloc(input_size);

  int16_t* farend_frame = (int16_t*)malloc(frame_length * 2);
  int16_t* echo_frame = (int16_t*)malloc(frame_length * 2);
  int16_t* out_frame = (int16_t*)malloc(frame_length * 2);

  const int16_t* farend_ptr = farend_frame;
  const int16_t* echo_ptr = echo_frame;
  float* tmp_farend = (float*)malloc(frame_length * 4);
  float* tmp_echo = (float*)malloc(frame_length * 4);
  // float* farend_float = tmp_farend;
  // float* echo_float = tmp_echo;
  std::vector<float> farend_float {tmp_farend, tmp_farend + frame_length};
  std::vector<float> echo_float {tmp_echo, tmp_echo + frame_length};

  aqa::ResidualEchoDetector echo_detector;


  while(input_size == wav_read_data(d_wav, input_buf, input_size)) {
    for (size_t i = 0; i < static_cast<size_t>(frame_length); ++i) {
      const uint8_t* in = &input_buf[2*i];
      echo_frame[i] = in[0] | (in[1] << 8);
    }
    
    if (input_size == wav_read_data(x_wav, input_buf, input_size)) {

      for (size_t i = 0; i < static_cast<size_t>(frame_length); ++i) {
        const uint8_t* in = &input_buf[2*i];
        farend_frame[i] = in[0] | (in[1] << 8);
      }

      aqa::S16ToFloat(farend_ptr, frame_length, tmp_farend);      
      echo_detector.AnalyzeRenderAudio(farend_float);
    }

    aqa::S16ToFloat(echo_ptr, frame_length, tmp_echo);      
    echo_detector.AnalyzeCaptureAudio(echo_float);
    
  }
/*
// ==============
  static constexpr int16_t kInput[] = {0, 1, -1, 16384, -16384, 32767, -32768};
  static constexpr float kReference[] = {
      0.f, 1.f / 32767.f, -1.f / 32768.f, 16384.f / 32767.f, -0.5f, 1.f, -1.f};
  static constexpr size_t kSize = 7;
  static_assert(7 == kSize, "");
  float output[kSize];
  aqa::S16ToFloat(kInput, kSize, output);
// ==============
  while (1) {
    int read, i;
    void *in_ptr, *out_ptr;
    uint8_t outbuf[20480];

    read = wav_read_data(x_wav, input_buf, input_size);
    // if (read != input_size) {
    if (read <= 0) {
      fprintf(stdout, "read [%d] != input_size [%d]\n", read, input_size);
      break;
    }
    for (i = 0; i < read/2; i++) {
      const uint8_t* in = &input_buf[2*i];
      convert_buf[i] = in[0] | (in[1] << 8);
    }
    in_ptr = convert_buf;

    out_ptr = outbuf;

    //fwrite(outbuf, 1, out_args.numOutBytes, out);
  }

*/
  auto ed_metrics = echo_detector.GetMetrics();
  std::cout << "echo_likelihood: " << ed_metrics.echo_likelihood
            << "\necho_likelihood_recent_max: " << ed_metrics.echo_likelihood_recent_max << std::endl;

  free(input_buf);
  free(convert_buf);
  free(farend_frame);
  free(echo_frame);
  free(out_frame);
  free(tmp_farend);
  free(tmp_echo);
  wav_read_close(x_wav);
  wav_read_close(d_wav);

  return 0;
}
