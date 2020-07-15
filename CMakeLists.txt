cmake_minimum_required(VERSION 3.12)

project(
  cpp-image-processing
  VERSION 0.0.1
  LANGUAGES CXX)

set(BIN_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(out main.cpp libpng_wrapper.cpp image_file_buffer.cpp)
set_target_properties(out PROPERTIES CXX_STANDARD 17 RUNTIME_OUTPUT_DIRECTORY
                                                     ${BIN_PATH})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

target_link_libraries(out PUBLIC ${PNG_LIBRARY})

# TODO: Find a cross-platform way to enable leak detection
#target_compile_options(out PUBLIC "-fsanitize=leak")

add_executable(
  testall catch_main.cpp libpng_wrapper.cpp image_file_buffer.cpp test_flatten.cpp test_image.cpp
          test_image_buffer.cpp test_image_traverser.cpp test_utility.cpp test_mat.cpp)
set_target_properties(testall PROPERTIES CXX_STANDARD 17
                                         RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

target_link_libraries(testall PUBLIC ${PNG_LIBRARY})
