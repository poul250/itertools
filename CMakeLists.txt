cmake_minimum_required(VERSION 3.0.0)
project(itertools VERSION 0.1.0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(itertools INTERFACE)
target_include_directories(itertools INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)
