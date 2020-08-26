cmake_minimum_required(VERSION 3.12)

project(
  cpp-image-processing
  VERSION 0.0.1
  LANGUAGES CXX)

set(BIN_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(
  out
  main.cpp
  libpng_wrapper.cpp
  gaussian.cpp
  canny.cpp
  sobel.cpp
  harris.cpp
  grayscale.cpp)
set_target_properties(out PROPERTIES CXX_STANDARD 17 RUNTIME_OUTPUT_DIRECTORY
                                                     ${BIN_PATH})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

target_link_libraries(out PUBLIC ${PNG_LIBRARY})

# TODO: Find a cross-platform way to enable optimize flags
target_compile_options(out PUBLIC "-Ofast")

add_executable(
  testall
  catch_main.cpp
  libpng_wrapper.cpp
  test_utility.cpp
  test_mat.cpp)
set_target_properties(testall PROPERTIES CXX_STANDARD 17
                                         RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

target_link_libraries(testall PUBLIC ${PNG_LIBRARY})
