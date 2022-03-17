#include "logging/apm_data_dumper.h"

#include <iostream>

namespace aqa {
namespace {
// ToDo(lifan): 
// #if defined(WEBRTC_WIN)
// constexpr char kPathDelimiter = '\\';
constexpr char kPathDelimiter = '/';
std::string FormFileName(const char* output_dir,
                         const char* name,
                         int instance_index,
                         const std::string& suffix) {

  std::string ss(name);
  return ss;
/*
  char buf[1024];
  std::string ss(buf);
  
  const size_t output_dir_size = strlen(output_dir);
  if (output_dir_size > 0) {
    ss.append(output_dir);
    if (output_dir[output_dir_size - 1] != kPathDelimiter) {
      ss.append(&kPathDelimiter);
    }
  }
  ss.append(name)
    .append("_")
    .append(std::to_string(instance_index))
    .append("-")
    .append(suffix);

  return ss; 
*/
}  

}  // namespace
ApmDataDumper::ApmDataDumper(int instance_index)
    : instance_index_(instance_index) {} 

ApmDataDumper::~ApmDataDumper() = default;

char ApmDataDumper::output_dir_[] = "";

FILE* ApmDataDumper::GetRawFile(const char* name) {
  std::string filename = FormFileName(output_dir_, name, instance_index_,
                                      ".pcm");
                                      //".dat");

  auto& f = raw_files_[filename];
  if (!f) {
    f.reset(fopen(filename.c_str(), "wb"));
    // RTC_CHECK(f.get()) << "Cannot write to  " << filename << ".";
  }
  return f.get();
}

}  // namespace aqa
