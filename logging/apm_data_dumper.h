#ifndef MODULES_AUDIO_PROCESSING_LOGGING_APM_DATA_DUMPER_H_
#define  MODULES_AUDIO_PROCESSING_LOGGING_APM_DATA_DUMPER_H_

#include <stdio.h>
#include <string.h>

#include <string>
#include <memory>
#include <unordered_map>

namespace aqa {

struct RawFileCloseFunctor {
  void operator()(FILE* f) const { fclose(f); }
};

class ApmDataDumper {
 public:
  explicit ApmDataDumper(int instance_index);

  ApmDataDumper() = delete;
  ApmDataDumper(const ApmDataDumper&) = delete;
  ApmDataDumper& operator=(const ApmDataDumper&) = delete;

  ~ApmDataDumper();

  static void SetOutputDirectory(const std::string& output_dir) {
    // ToDo(lifan): output_dir.size() < kOutputDirMaxLength 
    strncpy(output_dir_, output_dir.c_str(), output_dir.size());
  }

  void DumpRaw(const char* name, float v) {
    FILE* file = GetRawFile(name);
    fwrite(&v, sizeof(v), 1, file);
  }

  void DumpRaw(const char* name,
               size_t v_length,
               const float* v) {
    FILE* file = GetRawFile(name);
    fwrite(v, sizeof(v[0]), v_length, file);
  }

 private:

  static constexpr size_t kOutputDirMaxLength = 1024;
  static char output_dir_[kOutputDirMaxLength];
  const int instance_index_;
  std::unordered_map<std::string, std::unique_ptr<FILE, RawFileCloseFunctor>>
      raw_files_;
  FILE* GetRawFile(const char* name);
};

}  // namespace aqa

#endif  // MODULES_AUDIO_PROCESSING_LOGGING_APM_DATA_DUMPER_H_
