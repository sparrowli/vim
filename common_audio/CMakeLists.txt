
set(SOURCE
    include/audio_util.h
    audio_util.cc
    rtc_histogram.h
    rtc_histogram.cc)
    

add_library(common_audio ${SOURCE})
target_include_directories(common_audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
