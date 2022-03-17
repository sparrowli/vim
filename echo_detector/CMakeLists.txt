
set(echo_detector_SOURCE
    normalized_covariance_estimator.cc
    normalized_covariance_estimator.h
    moving_max.cc
    moving_max.h
    mean_variance_estimator.cc
    mean_variance_estimator.h
    circular_buffer.cc
    circular_buffer.h)

add_library(echo_detector ${echo_detector_SOURCE})
target_include_directories(echo_detector PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)

if (BUILD_GTEST)
  set (unittest_SOURCE
       ../logging/apm_data_dumper.h
       ../logging/apm_data_dumper.cc
       ../residual_echo_detector.cc
       ../residual_echo_detector.h
       ../residual_echo_detector_unittest.cc
       normalized_covariance_estimator_unittest.cc
       moving_max_unittest.cc
       circular_buffer_unittest.cc
       mean_variance_estimator_unittest.cc)
  add_executable(unit_test ${unittest_SOURCE})
  target_include_directories(unit_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
                                              ${CMAKE_CURRENT_SOURCE_DIR}/../)
  target_link_directories(unit_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/gtest)
  target_link_libraries(unit_test PUBLIC echo_detector
                                         common_audio
	                                 gtest
	                                 pthread
					 )
endif()
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

