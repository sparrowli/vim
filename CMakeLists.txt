# # CMake project for fdk-aac
#
# ## Basic usage
#
#    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF

cmake_minimum_required(VERSION 3.5.1)

# Policies

## Enables CMAKE_MSVC_RUNTIME_LIBRARY option support for CMake >= 3.15
## if you want to use a MSVC multi-threaded statically-linked runtime library
## If you enable it, CMake will build fdk-acc.dll without external dependencies.
##
## Example usage:
##
## cmake .. -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded$<$<CONFIG:Debug>:Debug>
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

project(audio_proc VERSION 0.0.1)

# Includes

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_VERSION VERSION_LESS 3.11)
  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
endif()

# Options

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(BUILD_PROGRAMS "Build aac-enc utility" OFF)
option(BUILD_GTEST "Bild gtest library" OFF)

# Checks

## Check if math functions are in separate library (Most of Linux distros, maybe some other OSes)
#check_function_exists(sin HAVE_DEFAULT_MATH)
#if(NOT HAVE_DEFAULT_MATH)
  #check_library_exists(m sin "" HAVE_LIBM)
#endif()

# Library

## Sources

#set(AACENC_SRC )  
  

#set(libfdk_aac_SOURCES
#    ${PCMUTILS_SRC} ${FDK_SRC} ${SYS_SRC})

#if(WIN32 AND BUILD_SHARED_LIBS)
#  set(libfdk_aac_SOURCES ${libfdk_aac_SOURCES} fdk-aac.def)
#endif()

## Create Library target. Actually fdk-aac is enough, but we also create
## FDK-AAC::fdk-aac for consistence with config-file package.

#add_library(fdk-aac ${libfdk_aac_SOURCES})
#add_library(FDK-AAC::fdk-aac ALIAS fdk-aac)

## Library target configuration

### Library target includes
#target_include_directories(fdk-aac
  #PUBLIC
    #$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libAACdec/include>
    #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libAACenc/include>
    #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libSYS/include>
  #PRIVATE
    #libArithCoding/include
    #libDRCdec/include
    #libSACdec/include
    #libSACenc/include
    #libSBRdec/include
    #libSBRenc/include
    #libMpegTPDec/include
    #libMpegTPEnc/include
    #libFDK/include
    #libPCMutils/include)
#
### Link math library if required
#target_link_libraries(fdk-aac PRIVATE $<$<BOOL:${HAVE_LIBM}>:m>)

### Set public headers and shared library version. Version info is critical for Unix-like OSes.
#set_target_properties(fdk-aac PROPERTIES
  #PUBLIC_HEADER "${fdk_aacinclude_HEADERS}"
  #VERSION 2.0.2
  #SOVERSION 2
  #MACHO_COMPATIBILITY_VERSION 3.0.0
  #MACHO_CURRENT_VERSION 3.2.0)
#
### Some compiler options from Makefile.am
#if(MSVC)
  #target_compile_options(fdk-aac PRIVATE /EHsc)
#else()
  #target_compile_options(fdk-aac PRIVATE -fno-exceptions -fno-rtti)
#endif()
#
#### Set proper name for MinGW or Cygwin DLL
#
#if((MINGW OR CYGWIN) AND BUILD_SHARED_LIBS)
  #set_property(TARGET fdk-aac PROPERTY RUNTIME_OUTPUT_NAME fdk-aac-2)
#endif()
#
## Library installation

### Note we export `fdk-aac-targets` to use with config-file package.

# Programs
if (BUILD_GTEST)

  # https://github.com/google/googletest/blob/main/googletest/README.md
endif()

if(BUILD_PROGRAMS)

  #[[
  set(echo_detector_SOURCE
   echo_detector/circular_buffer.cc
   echo_detector/circular_buffer.h)
  ]]
  add_subdirectory(common_audio)
  add_subdirectory(echo_detector)
  ## Program sources
  set(audio_proc_SOURCES
    main.cc
    logging/apm_data_dumper.h
    logging/apm_data_dumper.cc
    residual_echo_detector.cc
    residual_echo_detector.h
    wavreader.c
    wavreader.h
    wavwriter.c
    wavwriter.h)

  add_executable(audio_proc ${audio_proc_SOURCES} ${echo_detector_SOURCE})
  target_include_directories(audio_proc PUBLIC 
                             ${CMAKE_CURRENT_SOURCE_DIR}/
                             ${CMAKE_CURRENT_SOURCE_DIR}/include/)
  target_link_libraries(audio_proc PRIVATE
                                           common_audio
                                           echo_detector)


#[[
  set(aac_dec_SOURCES
    aac_dec.cc
    fdkaac_dec.cc
    fdkaac_dec.h
    wavwriter.c
    wavwriter.h)


  ## Program target
  add_executable(aac-enc ${aac_enc_SOURCES})
  target_include_directories(aac-enc PUBLIC src/include/)
  target_link_directories(aac-enc PUBLIC ../../codecs.packetizer/Products/default/Debian/release_for_Debian/x86_64/)
  target_link_libraries(aac-enc PRIVATE fdkaac)

  add_executable(aac-dec ${aac_dec_SOURCES})
  target_include_directories(aac-dec PUBLIC src/include/)
  target_link_directories(aac-dec PUBLIC ../../codecs.packetizer/Products/default/Debian/release_for_Debian/x86_64/)
  target_link_libraries(aac-dec PRIVATE fdkaac)
  ]]

endif()
